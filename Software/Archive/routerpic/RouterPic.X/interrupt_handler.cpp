#include <xc.h>
#include <sys/attribs.h>
#include <stdlib.h>
#include "interrupt_handler.h"
#include "buffer.h"
#include "constants.h"
#include "queue.h"
#include "FastTransfer.h"
#include "variables.h"

void __ISR(_UART_1_VECTOR, IPL1AUTO) Uart1Handler(void)
{
    while(U1STAbits.URXDA == 1)
    {
    ring_buff_put(&buffer_one, U1RXREG); //move data to software buffer
    }
    IFS0CLR = _IFS0_U1RXIF_MASK; // clear interrupt flag

    if(IFS0 & _IFS0_U1EIF_MASK)
    {
        U1STAbits.OERR = 0;
        IFS0CLR = _IFS0_U1EIF_MASK;
    }
}

void __ISR(_UART_2_VECTOR, IPL1AUTO) Uart2Handler(void)
{
    while(U2STAbits.URXDA == 1)
    {
    ring_buff_put(&buffer_two, U2RXREG);
    }
    IFS1CLR = _IFS1_U2RXIF_MASK;

    if(IFS1 & _IFS1_U2EIF_MASK)
    {
        U2STAbits.OERR = 0;
        IFS0CLR = _IFS1_U2EIF_MASK;
    }
}

void __ISR(_UART_3_VECTOR, IPL1AUTO) Uart3Handler(void)
{
    while(U3STAbits.URXDA == 1)
    {
    ring_buff_put(&buffer_three, U3RXREG);
    }
    IFS1CLR = _IFS1_U3RXIF_MASK;

    if(IFS1 & _IFS1_U3EIF_MASK)
    {
        U3STAbits.OERR = 0;
        IFS1CLR = _IFS1_U3EIF_MASK;
    }
}

void __ISR(_UART_4_VECTOR, IPL1AUTO) Uart4Handler(void)
{
    while(U4STAbits.URXDA == 1)
    {
    ring_buff_put(&buffer_four, U4RXREG);
    }
    IFS2CLR = _IFS2_U4RXIF_MASK;

    if(IFS2 & _IFS2_U4EIF_MASK)
    {
        U4STAbits.OERR = 0;
        IFS2CLR = _IFS2_U4EIF_MASK;
    }
}

void __ISR(_UART_5_VECTOR, IPL1AUTO) Uart5Handler(void)
{
    while(U5STAbits.URXDA == 1)
    {
    ring_buff_put(&buffer_five, U5RXREG);
    }
    IFS2CLR = _IFS2_U5RXIF_MASK;

    if(IFS2 & _IFS2_U5EIF_MASK)
    {
        U5STAbits.OERR = 0;
        IFS2CLR = _IFS2_U5EIF_MASK;
    }
}

void __ISR(_UART_6_VECTOR, IPL1AUTO) Uart6Handler(void)
{
    while(U6STAbits.URXDA == 1)
    {
    ring_buff_put(&buffer_six, U6RXREG);
    }
    IFS2CLR = _IFS2_U6RXIF_MASK;

    if(IFS2 & _IFS2_U6EIF_MASK)
    {
        U6STAbits.OERR = 0;
        IFS2CLR = _IFS2_U6EIF_MASK;
    }
}

//void __ISR(_TIMER_1_VECTOR, IPL7SRS) Timer1Handler(void){
//    static int count = 0;
//    count++;
//    if (count >= 100){
//        count = 0;
//        LATEbits.LATE4 ^= 1;
//    }
//    IFS0CLR = _IFS0_T1IF_MASK;
//}