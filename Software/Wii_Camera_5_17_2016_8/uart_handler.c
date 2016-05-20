#include <xc.h>
#include <stdbool.h>
#include <sys/attribs.h>
#include "uart_handler.h"


struct UART_ring_buff input_buffer;
struct UART_ring_buff output_buffer;
bool Transmit_stall = true;

void UART(void){

      U6MODEbits.BRGH = 0; // set to standard speed mode
    U6BRG = 42; //21;// 230400 baud   //42;// 115200 baud  //85;// 57600 baud
    U6MODEbits.PDSEL = 0b00; // 8-bit no parity
    U6MODEbits.STSEL = 0; // 1 stop bit
    IFS2bits.U6TXIF = 0;
    //IEC2bits.U6TXIE = 1; // transmit interrupt enable
    IPC12bits.U6IP = 1; // priority 1
    IPC12bits.U6IS = 1; // sub priority 1
    U6STAbits.UTXISEL = 0b01; // interrupt when transmit complete
//    U1STAbits.URXISEL = 0; // interrupt generated with every reception
    U6STAbits.URXEN = 1; // enable uart recieve
    U6STAbits.UTXEN = 1; // enable uart transmit
    U6MODEbits.ON = 1; // enable whole uart module
    // uart 4 receive lidar Data (only need RX wire)
    //disable analog part of the pins
    // uart 6 debug TX to computer terminal window (only need TX wire)
    U5MODEbits.BRGH = 0; // set to standard speed mode
    U5BRG = 42; //21;// 230400 baud   //42;// 115200 baud  //85;// 57600 baud
    U5MODEbits.PDSEL = 0b00; // 8-bit no parity
    U5MODEbits.STSEL = 0; // 1 stop bit
    IFS2bits.U5TXIF = 0;
    //IEC2bits.U6TXIE = 1; // transmit interrupt enable
    IPC12bits.U5IP = 1; // priority 1
    IPC12bits.U5IS = 1; // sub priority 1
    U5STAbits.UTXISEL = 0b01; // interrupt when transmit complete
//    U1STAbits.URXISEL = 0; // interrupt generated with every reception
    U5STAbits.URXEN = 1; // enable uart recieve
    U5STAbits.UTXEN = 1; // enable uart transmit
    U5MODEbits.ON = 1; // enable whole uart module
    // uart 6 error
//    IEC0bits.U1EIE = 1; // error interrupt enabed
    UART_buff_init(&input_buffer);
    UART_buff_init(&output_buffer);
}


//Called when Printf() is used
void _mon_putc(char c){
    if (U5STAbits.UTXBF == 1) { // uart transmit FIFO full
        int v;
        for(v=0; v<500; v++);
    }
    U5TXREG = c;
}



void UART_buff_init(struct UART_ring_buff* _this)
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

void UART_buff_put(struct UART_ring_buff* _this, const unsigned char c)
{
    if (_this->count < UART_BUFFER_SIZE)
    {
        _this->buf[_this->head] = c;
        _this->head = UART_buff_modulo_inc(_this->head, UART_BUFFER_SIZE);
        ++_this->count;
    } else
    {
        _this->buf[_this->head] = c;
        _this->head = UART_buff_modulo_inc(_this->head, UART_BUFFER_SIZE);
        _this->tail = UART_buff_modulo_inc(_this->tail, UART_BUFFER_SIZE);

    }
}

unsigned char UART_buff_get(struct UART_ring_buff* _this)
{
    unsigned char c;
    if (_this->count > 0)
    {
        c = _this->buf[_this->tail];
        _this->tail = UART_buff_modulo_inc(_this->tail, UART_BUFFER_SIZE);
        --_this->count;
    } else
    {
        c = 0;
    }
    return (c);
}

void UART_buff_flush(struct UART_ring_buff* _this, const int clearBuffer)
{
    _this->count = 0;
    _this->head = 0;
    _this->tail = 0;
    if (clearBuffer)
    {
        memset(_this->buf, 0, sizeof (_this->buf));
    }
}

int UART_buff_size(struct UART_ring_buff* _this)
{
    return (_this->count);
}

unsigned int UART_buff_modulo_inc(const unsigned int value, const unsigned int modulus)
{
    unsigned int my_value = value + 1;
    if (my_value >= modulus)
    {
        my_value = 0;
    }
    return (my_value);
}

unsigned char UART_buff_peek(struct UART_ring_buff* _this)
{
    return _this->buf[_this->tail];
}

unsigned char Receive_peek(void)
{
    return UART_buff_peek(&input_buffer);
}

int Receive_available(void)
{
    return UART_buff_size(&input_buffer);
}

unsigned char Receive_get(void)
{
    return UART_buff_get(&input_buffer);
}

void Send_put(unsigned char _data)
{

    //U6TXREG=_data;
    UART_buff_put(&output_buffer, _data);
    if((Transmit_stall == true) )//&& (UART_buff_size(&output_buffer) > 5))
    {
        Transmit_stall = false;
        IEC2bits.U5TXIE = 1;
        //U6TXREG = UART_buff_get(&output_buffer);
    }
}

//void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) //<----- DO NOT USE THIS CALL ... IT IS FOR dsPIC33  16bit
//{
//
//    //INDICATOR1=ON;
//    unsigned char data = U1RXREG;
//    UART_buff_put(&input_buffer, data);
//    IFS0bits.U1RXIF = 0; // Clear RX interrupt flag
//    //INDICATOR1=OFF;
//}

void __ISR(_UART_5_VECTOR, IPL1AUTO) _U5TXInterrupt(void)
{

         //LATBbits.LATB10^=1;
        if (UART_buff_size(&output_buffer) > 0)
        {
            U5TXREG = UART_buff_get(&output_buffer);
        }
        else
        {
            Transmit_stall = true;
            IEC2bits.U5TXIE = 0;
        }
         //INDICATOR2=OFF;

   // IFS2bits.U6TXIF = 0; // Clear TX interrupt flag
    IFS2CLR = _IFS2_U5TXIF_MASK;
}
