#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "uart_handler.h"


////Simple UART TX1 Called when Printf() is used (not using DMA)
//void _mon_putc(char c) {
//    if (U1STAbits.UTXBF == 1) { // uart transmit FIFO full
//        int v;
//        for(v=0; v<500; v++); //delay for a few clock cycles
//    }
//    U1TXREG = c;
//}



//Called when Printf() is used (working, but needs formatting for output)
void _mon_putc(char c) {
    static char dmaOutputArr[SIZE_OF_DMA_ARRAY];

    if(DMA_Counter > SIZE_OF_DMA_ARRAY) {
        DMA_Counter = 0;
        _queue_put(dmaOutputArr, sizeof(dmaOutputArr));
    }

    dmaOutputArr[DMA_Counter] = c; //copy the new char into the DMA temporary array
    DMA_Counter++;
}



//RX lidar receive
void __ISR(_UART_4_VECTOR, IPL1AUTO) Uart4Handler(void)
{
   //while(U4STAbits.URXDA == 1)
   // {
//        ring_buff_put(&buffer_five, U5RXREG);
        ring_buff_put(&buffer_four, U4RXREG);
   // }
    IFS2CLR = _IFS2_U4RXIF_MASK;

    if(IFS2 & _IFS2_U4EIF_MASK)
    {
        U4STAbits.OERR = 0;
        IFS2CLR = _IFS2_U4EIF_MASK;
    }
}


// initiate ring buffer
void ring_buff_init(struct ringBuff* _this)
{
    /*****
      The following clears:
        -> buf
        -> head
        -> tail
        -> count
      and sets head = tail
     ***/
    memset(_this, 0, sizeof (*_this));
}


//place a character into the ring buffer
void ring_buff_put(struct ringBuff* _this, const unsigned char c)
{
    if (_this->count < RING_BUF_SIZE)
    {
        _this->buf[_this->head] = c;
        _this->head = modulo_inc(_this->head, RING_BUF_SIZE);
        ++_this->count;
    } else
    {
        _this->buf[_this->head] = c;
        _this->head = modulo_inc(_this->head, RING_BUF_SIZE);
        _this->tail = modulo_inc(_this->tail, RING_BUF_SIZE);

    }
}


//get a character from the ring buffer
unsigned char ring_buff_get(struct ringBuff* _this)
{
    unsigned char c;
    if (_this->count > 0)
    {
        c = _this->buf[_this->tail];
        _this->tail = modulo_inc(_this->tail, RING_BUF_SIZE);
        --_this->count;
    } else
    {
        c = 0;
    }
    return (c);
}


//returns the tail caracter without removing it from the buffer
unsigned char ring_buff_peek(struct ringBuff* _this)
{
    return (_this->buf[_this->tail]);
}


// flushes the ring buffer
void ring_buff_flush(struct ringBuff* _this, const int clearBuffer)
{
    _this->count = 0;
    _this->head = 0;
    _this->tail = 0;
    if (clearBuffer)
    {
        memset(_this->buf, 0, sizeof (_this->buf));
    }
}


// returns the size of the data in the buffer
int ring_buff_size(struct ringBuff* _this)
{
    return (_this->count);
}


// used to service the subrutiens of the ring buffer
unsigned int modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}