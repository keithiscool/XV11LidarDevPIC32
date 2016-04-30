#include <xc.h>
#define _SUPPRESS_PLIB_WARNING
#include <plib.h>
#include "queue.h"
#include "initialize.h"



//// prototype for setting to pass to dma class
//struct dmaSettings
//{
//    unsigned char *dma_array;
//    volatile unsigned int *dmacon;
//    unsigned int con_busy_mask;
//    unsigned int con_en_mask;
//    volatile unsigned int *dmasize;
//    volatile unsigned int *dmaecon;
//    unsigned int econ_force_mask;
//};


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
//Output Compare Module (OC4) for U1TX controls the PWM for the Lidar motor
//Fosc==80MHz
void PWM(void){
    T4CONbits.ON = 0;
    T4CONbits.T32 = 0; //16 bit mode
    T4CONbits.TCKPS = 0b001; //prescalar of 2
    T4CONbits.TGATE = 0;
    PR4 = 3800;
    OC4CONbits.ON = 0;
    OC4CONbits.OCM = 0b110; //fault pin disabled
    OC4R = 2180; // near 200rpm
    OC4RS = 2180; // near 200rpm
    T4CONbits.ON = 1;
    OC4CONbits.ON = 1;
}


void UART(void){
    // uart 4 receive lidar Data (only need RX wire)
    //disable analog part of the pins
//    AD1PCFGbits.PCFG8 = 1; //disable analog on RX pin
//    AD1PCFGbits.PCFG14 = 1; //disable analog on TX5 pin
    U4MODEbits.BRGH = 0; // set to standard speed mode
    U4BRG = 42; // 115200 baud  //85; // 57600 baud
    U4MODEbits.PDSEL = 0b00; // 8-bit no parity
    U4MODEbits.STSEL = 0; // 1 stop bit
    IEC2bits.U4RXIE = 1; // enable uart recieve
    IPC12bits.U4IP = 1; // priority 1
    IPC12bits.U4IS = 2; // sub priority 2
    U4STAbits.URXEN = 1; // enable uart recieve
    U4STAbits.URXISEL = 0b00; // interrupt generated with every reception
    U4STAbits.UTXEN = 1; // enable uart transmit
    U4MODEbits.ON = 1; // enable whole uart module
    // uart 4 error
    IEC2bits.U4EIE = 1; // error interrupt enabed

    // uart 6 debug TX to computer terminal window (only need TX wire)
    U6MODEbits.BRGH = 0; // set to standard speed mode
    U6BRG = 21; //21;// 230400 baud   //42;// 115200 baud  //85;// 57600 baud
    U6MODEbits.PDSEL = 0b00; // 8-bit no parity
    U6MODEbits.STSEL = 0; // 1 stop bit
    IFS2bits.U6TXIF = 0;
//    IEC0bits.U1TXIE = 1; // transmit interrupt enable
    IPC12bits.U6IP = 1; // priority 1
    IPC12bits.U6IS = 1; // sub priority 1
//    U1STAbits.UTXISEL = 0b01; // interrupt when transmit complete
//    U1STAbits.URXISEL = 0; // interrupt generated with every reception
    U6STAbits.URXEN = 1; // enable uart recieve
    U6STAbits.UTXEN = 1; // enable uart transmit
    U6MODEbits.ON = 1; // enable whole uart module
    // uart 6 error
//    IEC0bits.U1EIE = 1; // error interrupt enabed
}


void DMA(void) {
    DMACONbits.ON = 1; // dma module enabled
    DCRCCON = 0; // crc module disabled

    //dma 1 for U1TX debugging
    DCH1CONbits.CHPRI = 2; // channel priority 2
    DCH1ECONbits.CHSIRQ = 72; // uart 6 tx Interrupt "IRQ" Vector Number (p.134)
    DCH1ECONbits.SIRQEN = 1; // enable cell transfer when IRQ triggered
    DCH1INT = 0; // clear all existing flags, disable all interrupts
    DCH1SSA = (unsigned int) &dma_one_array & 0x1FFFFFFF; // physical address conversion for transmit buffer
    DCH1DSA = (unsigned int) &U6TXREG & 0x1FFFFFFF; // physical address conversion for uart send buffer
    DCH1DSIZ = 1;
    DCH1CSIZ = 1;

    
    //DMA 1 settings
    arrayOFdmaSetting[1].dma_array = (unsigned char*) &dma_one_array;
    arrayOFdmaSetting[1].dmacon = &DCH1CON;
    arrayOFdmaSetting[1].con_busy_mask = _DCH1CON_CHBUSY_MASK;
    arrayOFdmaSetting[1].con_en_mask = _DCH1CON_CHEN_MASK;
    arrayOFdmaSetting[1].dmasize = &DCH1SSIZ;
    arrayOFdmaSetting[1].dmaecon = &DCH1ECON;
    arrayOFdmaSetting[1].econ_force_mask = _DCH1ECON_CFORCE_MASK;

    
    //After loading the DMA settings, start using the DMA
//    queue_pointer[1] = &DMA_one;
    queue_begin(arrayOFdmaSetting, 1);
//    queue_begin(arrayOFdmaSetting, 1);

}