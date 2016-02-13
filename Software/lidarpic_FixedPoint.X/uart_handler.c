#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "uart_handler.h"
//STICKY INTERRUPTS:: Some interrupts, including UART&ADC on pic32
//are sticky which means that you have to remove the cause
//of the interrupt before clearing the IFS# flag

//Simple putchar function called when Printf() is used
void _mon_putc(char c){
    if (U1STAbits.UTXBF == 1) { // uart transmit FIFO full
        int v;
        for(v=0; v<1000; v++);
    }
    U1TXREG = c;
}

//////////Interrupt Driven by Printf to UART 1 (debug output to PC) (UART1 is not showing data 0-360 distance points)
//////////This is only showing Distances 272-360
////////void _mon_putc(char c){
////////    if(secondTransmission == false) { //place first byte into ring buffer so that interrupt will work properly
////////        ring_buff_put(&buffer_one, c); //first byte gets stored in U1TX ring buffer (buffer_one)
////////        secondTransmission = true;
////////        return;
////////    }
////////
////////    if((secondTransmission == true) && (serialTransmission == false)) {
////////        ring_buff_put(&buffer_one, c); //second byte gets stored in U1TX ring buffer (buffer_one)
////////        U1TXREG = &buffer_one.head; //send first byte after the second byte is stored in the U1TX ring buffer (buffer_one)
////////        return;
////////    }
////////
////////    if (stalled == true){
////////        serialTransmission = true; //operate the ring buffer normally
////////        stalled = false;
////////        U1TXREG = c;
////////        //IEC0bits.U1TXIE = 1; // enable interrupt
////////    }
////////    else{
////////        ring_buff_put(&buffer_one, c); //fill ring buffer if U1TXREG hardware FIFO is not empty
////////    }
////////}
////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////
//////////TX computer debugging (KEITH VERSION)
////////void __ISR(_UART_1_VECTOR, IPL1AUTO) Uart1Handler(void) { //Copies bytes to the local UART TX1 from ring buffer
////////    static unsigned int count = 0;
////////    count ++;
////////
////////    LATEbits.LATE3 ^= 1; //test 2 LED
////////    if(ring_buff_size(&buffer_one) > 0) {
////////        U1TXREG = ring_buff_get(&buffer_one);
////////        IFS0bits.U1TXIF = 0; //Clear the TX interrupt flag before transmitting again
////////    }
////////
////////    else {
////////        IEC0bits.U1TXIE = 0; // Disable the TX interrupt if we are done so that we don't keep entering this ISR
////////        stalled = true;
////////        memset(&buffer_one, 0, RING_BUF_SIZE); //set all elements in ring buffer to zero
////////        secondTransmission = false; //reset the first two byte flag
////////    }
////////    IFS0CLR = _IFS0_U1TXIF_MASK;
////////
////////    if(IFS0 & _IFS0_U1EIF_MASK) {
////////        U1STAbits.OERR = 0; // OERR == 0: Receive buffer has not overflowed
////////        IFS0CLR = _IFS0_U1EIF_MASK;
////////    }
////////}
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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//RX lidar receive
void __ISR(_UART_5_VECTOR, IPL1AUTO) Uart5Handler(void)
{
   //while(U5STAbits.URXDA == 1)
   // {
        ring_buff_put(&buffer_five, U5RXREG);
   // 

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
void ring_buff_put(struct ringBuff* _this, const unsigned char c) {
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