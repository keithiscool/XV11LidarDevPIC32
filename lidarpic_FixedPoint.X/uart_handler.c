#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "uart_handler.h"


//Called when Printf() is used
void _mon_putc(char c){
    if (U1STAbits.UTXBF == 1) { // uart transmit FIFO full
        int v;
        for(v=0; v<1000; v++);
    }
    U1TXREG = c;
}

//Interrupt Driven Printf to UART 1 (debug output to PC) is not showing data 0-360 distance points
//This is only showing Distances 272-360
//void _mon_putc(char c){
//    if (stalled == true){
//        stalled = false;
//        U1TXREG = c;
//        //IEC0bits.U1TXIE = 1; // enable interrupt
//    }
//    else{
//        ring_buff_put(&buffer_one, c);
//    }
//}
////TX computer debugging
//void __ISR(_UART_1_VECTOR, IPL1AUTO) Uart1Handler(void)
//{
//    static unsigned int count = 0;
//    count ++;
//
//    LATEbits.LATE3 ^= 1; //test 2
//    if(ring_buff_size(&buffer_one) > 0)
//    {
//        U1TXREG = ring_buff_get(&buffer_one);
//    }
//    else
//    {
//        IEC0bits.U1TXIE = 0; // disable interrupt
//        stalled = true;
//    }
//    IFS0CLR = _IFS0_U1TXIF_MASK;
//
//    if(IFS0 & _IFS0_U1EIF_MASK)
//    {
//        U1STAbits.OERR = 0;
//        IFS0CLR = _IFS0_U1EIF_MASK;
//    }
//}



//RX lidar receive
void __ISR(_UART_5_VECTOR, IPL1AUTO) Uart5Handler(void)
{
   //while(U5STAbits.URXDA == 1)
   // {
        ring_buff_put(&buffer_five, U5RXREG);
   // }
    IFS2CLR = _IFS2_U5RXIF_MASK;

    if(IFS2 & _IFS2_U5EIF_MASK)
    {
        U5STAbits.OERR = 0;
        IFS2CLR = _IFS2_U5EIF_MASK;
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