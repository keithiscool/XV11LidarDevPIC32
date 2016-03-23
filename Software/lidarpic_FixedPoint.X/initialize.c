#include <xc.h>
#define _SUPPRESS_PLIB_WARNING
#include <plib.h>
#include "initialize.h"

void initialize(void){
     SYSTEMConfig(80000000, SYS_CFG_ALL); // sets up periferal and clock configuration
     INTEnableSystemMultiVectoredInt();
     delay();
     timers();
     delay();
     PWM();
     delay();
     UART();
     delay();
     DMA();
     delay();
     INTEnableInterrupts(); // enable interrupts
     beginLIDARdecoder(returned_data, &buffer_five);
}

void timers(void){
    T1CONbits.ON = 0;
    T1CONbits.TCS = 0; // perifial clock as source
    T1CONbits.TCKPS = 0b10; //64 prescalar
    T1CONbits.TGATE = 0;
    PR1 = 1250;
    IFS0CLR = _IFS0_T1IF_MASK;
    IEC0SET = _IEC0_T1IE_MASK;
    IPC1SET = ((0x1 << _IPC1_T1IP_POSITION) | (0x1 << _IPC1_T1IS_POSITION));
    T1CONbits.ON = 1;
}

//Control the speed the lidar rotates (alter speed to achieve around 200rpm)
//Fosc==80MHz
void PWM(void){
    T2CONbits.ON = 0;
    T2CONbits.T32 = 0; //16 bit mode
    T2CONbits.TCKPS = 0b001; //prescalar of 2
    T2CONbits.TGATE = 0;
//    PR2 = 4000;
    PR2 = 3800;
    OC2CONbits.ON = 0;
    OC2CONbits.OCM = 0b110;
    OC2R = 2180; // near 200rpm
    OC2RS = 2180; // near 200rpm
    T2CONbits.ON = 1;
    OC2CONbits.ON = 1;
}

void UART(void){
    // uart 5 receive lidar Data (only need RX wire)
    AD1PCFGbits.PCFG8 = 1; //disable analog on RX pin
    AD1PCFGbits.PCFG14 = 1; //disable analog on TX5 pin
    U5MODEbits.BRGH = 0; // set to standard speed mode
    U5BRG = 42; // 115200 baud  //85; // 57600 baud
    U5MODEbits.PDSEL = 0b00; // 8-bit no parity
    U5MODEbits.STSEL = 0; // 1 stop bit
    IEC2bits.U5RXIE = 1; // enable uart recieve
    IPC12bits.U5IP = 1; // priority 1
    IPC12bits.U5IS = 2; // sub priority 2
    U5STAbits.URXEN = 1; // enable uart recieve
    U5STAbits.URXISEL = 0b00; // interrupt generated with every reception
    U5STAbits.UTXEN = 1; // enable uart transmit
    U5MODEbits.ON = 1; // enable whole uart module
    // uart 5 error
    IEC2bits.U5EIE = 1; // error interrupt enabed

    // uart 1 debug TX to computer terminal window
    U1MODEbits.BRGH = 0; // set to standard speed mode
    U1BRG = 21; //21;230400 baud   //42;// 115200 baud  //85;// 57600 baud
    U1MODEbits.PDSEL = 0b00; // 8-bit no parity
    U1MODEbits.STSEL = 0; // 1 stop bit
    IFS0bits.U1TXIF = 0;
//    IEC0bits.U1TXIE = 1; // transmit interrupt enable
    IPC6bits.U1IP = 1; // priority 1
    IPC6bits.U1IS = 1; // sub priority 1
//    U1STAbits.UTXISEL = 0b01; // interrupt when transmit complete
//    U1STAbits.URXISEL = 0; // interrupt generated with every reception
    U1STAbits.URXEN = 1; // enable uart recieve
    U1STAbits.UTXEN = 1; // enable uart transmit
    U1MODEbits.ON = 1; // enable whole uart module
    // uart 1 error
//    IEC0bits.U1EIE = 1; // error interrupt enabed
}

void DMA(void) {
    DMACONbits.ON = 1; // dma module enabled
    DCRCCON = 0; // crc module disabled

    //dma 1 for U1TX debugging
    DCH1CONbits.CHPRI = 2; // channel priority 2
    DCH1ECONbits.CHSIRQ = 28; // uart 1 tx IRQ
        
    DCH1ECONbits.SIRQEN = 1; // enable cell transfer when IRQ triggered
    DCH1INT = 0; // clear all existing flags, disable all interrupts
    DCH1SSA = (unsigned int) &dma_one_array & 0x1FFFFFFF; // physical address conversion for transmit buffer
    DCH1DSA = (unsigned int) &U1TXREG & 0x1FFFFFFF; // physical address conversion for uart send buffer
    DCH1DSIZ = 1;
    DCH1CSIZ = 1;

    
    //DMA 1 settings
    dmaOneSettings.dma_array = (unsigned char*) &dma_one_array;
    dmaOneSettings.dmacon = &DCH1CON;
    dmaOneSettings.con_busy_mask = _DCH1CON_CHBUSY_MASK;
    dmaOneSettings.con_en_mask = _DCH1CON_CHEN_MASK;
    dmaOneSettings.dmasize = &DCH1SSIZ;
    dmaOneSettings.dmaecon = &DCH1ECON;
    dmaOneSettings.econ_force_mask = _DCH1ECON_CFORCE_MASK;

    
    //After loading the DMA settings, start using the DMA
    _queue_begin(&dmaOneSettings, 1);
    queue_One_Pointer = &DMA_Buffer_One;
}