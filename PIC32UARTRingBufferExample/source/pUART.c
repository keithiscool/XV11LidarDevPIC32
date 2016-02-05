/*
 * File:   ringuart.c
 * Author: Phoenix
 *
 * Created on July 22, 2013, 7:25 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <psystem.h>
#include <pUART.h>
#include <apiTick.h>

void ConfigUART(void);
void PutStrUART(char *pcString);

static BYTE vUARTMTXFIFO[uUART_TX_BUFFER_SIZE] = {0};

static volatile BYTE *MTXHeadPtr = vUARTMTXFIFO, *MTXTailPtr = vUARTMTXFIFO;

/*********************************************************************
 * Function:        void UART2TCPBridgeInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Sets up the UART peripheral for this application
 *
 * Note:            Uses interrupts
 ********************************************************************/
void ConfigUART(void)
{
    mPORTGClearBits(BIT_7);
    mPORTGSetPinsDigitalOut(BIT_7);

    // set hardware flow control
    UARTConfigure(COM_UART_MODULE_3, UART_ENABLE_PINS_TX_RX_ONLY);
    // set baudrate
    UARTSetDataRate(COM_UART_MODULE_3, GetSystemClock(), COM_UART_RATE_19K);
    // set line control
    UARTSetLineControl(COM_UART_MODULE_3, COM_UART_MODE_8N1);
    // set FIFO mode
    UARTSetFifoMode(COM_UART_MODULE_3, UART_INTERRUPT_ON_RX_NOT_EMPTY | UART_INTERRUPT_ON_TX_DONE);
    // enable peripheral, transmitter and receiver
    UARTEnable(COM_UART_MODULE_3, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_TX));

    // configure UART for TX/RX...
    INTSetVectorPriority(INT_VECTOR_UART(COM_UART_MODULE_3), INT_PRIORITY_LEVEL_5);
    INTSetVectorSubPriority(INT_VECTOR_UART(COM_UART_MODULE_3), INT_SUB_PRIORITY_LEVEL_1);
    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR);
}

void PutStrUART(char *pcString)
{
    BYTE *MTXHeadPtrShadow, *MTXTailPtrShadow;

    // Read FIFO pointers into a local shadow copy.  Some pointers are volatile
    // (modified in the ISR), so we must do this safely by disabling interrupts
    MTXHeadPtrShadow = (BYTE*)MTXHeadPtr;
    IEC1bits.U3TXIE = 0;
    MTXTailPtrShadow = (BYTE*)MTXTailPtr;

    if(MTXHeadPtrShadow != MTXTailPtrShadow)
        IEC1bits.U3TXIE = 1;

    while(*pcString)
    {
        *MTXHeadPtrShadow++ = *pcString;            // Write character
        pcString++;

        if(MTXHeadPtrShadow >= vUARTMTXFIFO + sizeof(vUARTMTXFIFO))
        MTXHeadPtrShadow = vUARTMTXFIFO;
    }

    // Write local shadowed FIFO pointers into the volatile FIFO pointers.
    IEC1bits.U3TXIE = 0;
    MTXHeadPtr = (volatile BYTE*)MTXHeadPtrShadow;
    
    if(MTXHeadPtrShadow != MTXTailPtrShadow)
        IEC1bits.U3TXIE = 1;
}

/*********************************************************************
 * Function:        void _ISR _U3TXInterrupt(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        Copies bytes to and from the local UART TX and
 *					RX FIFOs
 *
 * Note:            None
 ********************************************************************/
void _U3TXInterrupt(void)
{
    // Transmit a byte, if pending, if possible
    if(MTXHeadPtr != MTXTailPtr)
    {
        // Clear the TX interrupt flag before transmitting again
        IFS1bits.U3TXIF = 0;

        U3TXREG = *MTXTailPtr++;
        if(MTXTailPtr >= vUARTMTXFIFO + sizeof(vUARTMTXFIFO))
            MTXTailPtr = vUARTMTXFIFO;
    }
    else	// Disable the TX interrupt if we are done so that we don't keep entering this ISR
    {
        IEC1bits.U3TXIE = 0;
        memset(vUARTMTXFIFO, 0, sizeof(vUARTMTXFIFO));
    }
}

void __ISR(_UART_3_VECTOR, ipl5) U3Interrupt(void)
{
    if(IEC1bits.U3TXIE)
    {
        if(IFS1bits.U3TXIF)
            _U3TXInterrupt();
    }
}
