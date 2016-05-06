#include <xc.h>
#include <string.h>
#include "queue.h"
#include <sys/attribs.h>


// declaration for dma settings containing settings for all dmas
struct dmaSettings arrayOFdmaSetting[1];


// declarations for dma arrays for transmit
unsigned char dma_one_array[SIZE_OF_DMA_ARRAY];



//////////////void __ISR(_UART_4_VECTOR, IPL1AUTO) Uart4Handler(void)
////////////void __ISR(_DMA_1_VECTOR, IPL2AUTO) __DMA1Interrupt(void)
////////////{
//////////////    int dmaFlags=DCH1INT&0xff; // read the interrupt flags
////////////    /*
////////////    perform application specific operations in response to any interrupt flag set
////////////    */
////////////
////////////    LATBbits.LATB9 = 1; //off LED
////////////    LATBbits.LATB10 = 1; //off LED
////////////    LATBbits.LATB11 = 0; //on LED
////////////    while(1);
////////////
//////////////    DCH1INTCLR=0x000000ff; // clear the DMA channel interrupt flags
////////////    IFS1bits.DMA1IF = 0;
////////////    IFS1CLR = _IFS1_DMA1IF_MASK; // Be sure to clear the DMA0 interrupt flags
////////////    // before exiting the service routine.
////////////}



// function runs to iniciate the queue system
// the function is handed all the tools it needs to work the dma for sending
void queue_begin(struct dmaSettings *_settings, int module_number)
{
    // move settings to internal variables
    dma_array = _settings[module_number].dma_array;
    dmacon = _settings[module_number].dmacon;
    con_busy_mask = _settings[module_number].con_busy_mask;
    con_en_mask = _settings[module_number].con_en_mask;
    dmasize = _settings[module_number].dmasize;
    dmaecon = _settings[module_number].dmaecon;
    econ_force_mask = _settings[module_number].econ_force_mask;
    module_ID = module_number;

    // flush the queue buffer
    queue_init();
}


// set everything to zeros
void queue_init(void)
{
    send_queue.head = 0;
    send_queue.tail = 0;
    send_queue.count = 0;
    int i;
    for (i = 0; i < TOP_LEVEL_QUEUE_DEPTH; i++)
    {
        send_queue.queue[i].count = 0;
        send_queue.queue[i].head = 0;
        send_queue.queue[i].tail = 0;
        memset(send_queue.queue[i].buf, 0, SECOND_LEVEL_QUEUE_DEPTH);
    }
}


//void queue_put(unsigned char * what, unsigned char how_many, unsigned char where, unsigned char from_where)
void queue_put(unsigned char * what, unsigned int how_many)
{
    if (send_queue.count < TOP_LEVEL_QUEUE_DEPTH)
    {
        unsigned int i;
        for (i = 1; i < how_many; i++)
        {
            queue_data_put(what[i]);
        }
        send_queue.head = modulo_inc_dma(send_queue.head, TOP_LEVEL_QUEUE_DEPTH);
        send_queue.count++;
    } else
    {
        int i;
        
        for (i = 1; i < how_many; i++)
        {
            queue_data_put(what[i]);
        }
        send_queue.head = modulo_inc_dma(send_queue.head, TOP_LEVEL_QUEUE_DEPTH);
        send_queue.tail = modulo_inc_dma(send_queue.tail, TOP_LEVEL_QUEUE_DEPTH);
        queueFullErrorCounter++; // because the queue is full to use this functions
    }
}


void queue_data_put(unsigned char data)
{
    if (send_queue.queue[send_queue.head].count < SECOND_LEVEL_QUEUE_DEPTH)
    {
        send_queue.queue[send_queue.head].buf[send_queue.queue[send_queue.head].head] = data;
        send_queue.queue[send_queue.head].head = modulo_inc_dma(send_queue.queue[send_queue.head].head, SECOND_LEVEL_QUEUE_DEPTH);
        send_queue.queue[send_queue.head].count++;
    } else
    {
        send_queue.queue[send_queue.head].buf[send_queue.queue[send_queue.head].head] = data;
        send_queue.queue[send_queue.head].head = modulo_inc_dma(send_queue.queue[send_queue.head].head, SECOND_LEVEL_QUEUE_DEPTH);
        send_queue.queue[send_queue.head].tail = modulo_inc_dma(send_queue.queue[send_queue.head].tail, SECOND_LEVEL_QUEUE_DEPTH);
    }
}


unsigned char queue_data_get(void)
{
    unsigned char dat;
    if (send_queue.count > 0)
    {
        if (send_queue.queue[send_queue.tail].count > 0)
        {
            dat = send_queue.queue[send_queue.tail].buf[send_queue.queue[send_queue.tail].tail];
            send_queue.queue[send_queue.tail].tail--;
            send_queue.queue[send_queue.tail].count--;
        } else
        {
            dat = 0;
        }
    } else
    {
        dat = 0;
    }
    return dat;
}



unsigned int modulo_inc_dma(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}



//place this function at end of main after the printf() functions are called (after dma ring buffer is filled)
bool queue_send(void)
{
    if (send_queue.count > 0) // if send queue is empty no need to send
    {
        if ((*dmacon & con_busy_mask) == 0) // if dma is busy then no need to send
        {
            // if all the checks pass populate the dma buffer, set the size, enable and force a send
            int g;
            int h = send_queue.queue[send_queue.tail].count;
            for (g = 0; g < h; g++)
            {
                dma_array[g] = send_queue.queue[send_queue.tail].buf[g];
            }

            //kick the DMA
            *dmasize = h;
            *dmacon |= con_en_mask;
            *dmaecon |= econ_force_mask;

            // flush send queue
            send_queue.queue[send_queue.tail].count = 0;
            send_queue.queue[send_queue.tail].head = 0;
            send_queue.queue[send_queue.tail].tail = 0;
            memset(send_queue.queue[send_queue.tail].buf, 0, SECOND_LEVEL_QUEUE_DEPTH);
            
            //tidy up tail and counter
            send_queue.count--;
            send_queue.tail = modulo_inc_dma(send_queue.tail, TOP_LEVEL_QUEUE_DEPTH);
            return true;
        } else
        {
            return false;
        }
    } else
    {
        return false;
    }

}


// returns the queue full error
unsigned int fullError(void)
{
    return queueFullErrorCounter;
}