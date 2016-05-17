#include <xc.h>
#define _SUPPRESS_PLIB_WARNING
#include <plib.h>
#include "queue.h"
#include "initialize.h"



void initialize(void){
     SYSTEMConfig(80000000, SYS_CFG_ALL); // sets up periferal and clock configuration
     INTEnableSystemMultiVectoredInt();
     IOpins();
     delay();
//     timers();
//     delay();
     PWM();
     delay();
     UART();
     delay();
     DMA();
     delay();
     INTEnableInterrupts(); //enable interrupts
     beginLIDARdecoder(returned_data, &buffer_five);
}


void IOpins(void) {
    DDPCONbits.JTAGEN = 0;  // This turns off the JTAG and allows PORTA pins to be used (Must disable the JTAG module in order to use LAT registers to set discrete outputs (LEDs)

    TRISEbits.TRISE4 = 1;   // set output debugging LEDs for PIC32 Dev Board
    TRISEbits.TRISE3 = 1;   // set output debugging LEDs for PIC32 Dev Board

    TRISBbits.TRISB8 = 1;   // U5RX TO LANTRONIX
    TRISBbits.TRISB9 = 0;   // set output debugging LEDs
    TRISBbits.TRISB10 = 0;  // set output debugging LEDs
    TRISBbits.TRISB11 = 0;  // set output debugging LEDs

    TRISBbits.TRISB12 = 0;  // SERVO 1 CONTROL PWM and debug LED
    TRISBbits.TRISB13 = 0;  // SERVO 2 CONTROL PWM and debug LED

    TRISBbits.TRISB14 = 1;  // U5TX TO LANTRONIX

    TRISBbits.TRISB2 = 1;   //ANALOG PINS
    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;
//    AD1PCFG = 0xEFFF; //disable Analog Select for B pins
    TRISBbits.TRISB6 = 1;
    TRISBbits.TRISB7 = 1;

    TRISE = 0xFFFFFF;

    TRISDbits.TRISD1 = 0;   //UART 4 TX
    TRISDbits.TRISD2 = 0;   //I2C 2 Header (I2C SDA3)
//    TRISDbits.TRISD3 = 1;   //I2C 2 Header (I2C SCL3)
//    TRISDbits.TRISD3 = 0;   //(TIMER3 output for motor) I2C 2 Header (I2C SCL3)
    TRISDbits.TRISD4 = 1;   //Servo A
    TRISDbits.TRISD5 = 1;   //Servo B
    TRISDbits.TRISD6 = 1;   //Servo C
    TRISDbits.TRISD7 = 1;   //Servo D
    TRISDbits.TRISD8 = 1;
    TRISDbits.TRISD9 = 1;   //UART 4 RX (LIDAR Input)
    TRISDbits.TRISD10 = 1;
    TRISDbits.TRISD11 = 1;
    
    TRISFbits.TRISF0 = 1;
    TRISFbits.TRISF1 = 1;
    TRISFbits.TRISF2 = 1;
    TRISFbits.TRISF3 = 1;
    TRISFbits.TRISF4 = 0;   //I2C 1 Header (I2C SDA5)
    TRISFbits.TRISF5 = 1;   //I2C 1 Header (I2C SCL5)

    TRISG = 0xFFFFFF;       //LANTRONIX PINS (2 SETS OF UART - ONE FOR LANTRONIX AND 1 FOR DEBUG)
    
}



//100ms timer (PR = 31250, prescaler = 256)
void timers(void){
    T3CONbits.ON = 0;
//    T3CONbits.TCS = 0; // peripheral clock as source
    T3CONbits.TCKPS = 0b111; //256 prescalar
//    T3CONbits.TCKPS = 0b110; //64 prescalar
//    T3CONbits.TCKPS = 0b001; //2 prescalar
    T3CONbits.TGATE = 0;
    //TPB_clock is the clock resource for peripherals on pic32MX (p.201) (look at system clock / "FPBDIV") - located in configuration bits
    //Example:
    //Desired_Period == 100ms
    //PR# = Desired_Period / [(1/Fosc)*Prescaler]
    //PR# = 0.100seconds / [(1/80MHz)*256] = 31250
    PR3 = 31250;
    IFS0CLR = _IFS0_T3IF_MASK;
    IEC0SET = _IEC0_T3IE_MASK;
    IPC3SET = ((0x1 << _IPC3_T3IP_POSITION) | (0x1 << _IPC3_T3IS_POSITION));
    T3CONbits.ON = 1;
}



//Control the speed the lidar rotates (alter speed to achieve around 200rpm)
//The PWM uses Output Compare 1 (pin 46) using timer 2 (timer 1 is different than other timers)
//Fosc==80MHz
void PWM(void){
    T2CONbits.ON = 0;
    T2CONbits.T32 = 0; //16 bit mode
    T2CONbits.TCKPS = 0b001; //prescalar of 2
    T2CONbits.TGATE = 0;
    //PR# = Desired_Period / [(1/Fosc)*Prescaler]
    //PR# = (1/10.5kHz) / [(1/80MHz)*2] = 3800
    PR2 = 3800; //around 10.5kHz frequency
    OC1CONbits.ON = 0;
    OC1CONbits.OCM = 0b110;
    OC1CONbits.OCTSEL = 0; //select Timer 2 as source for OC1 (PWM)
//    OC1R = 2100; // near 230rpm
//    OC1RS = 2100; // near 230rpm
//    OC1R = 2300; // near 230rpm
//    OC1RS = 2300; // near 230rpm
    OC1R = 2450; // near 260rpm
    OC1RS = 2450; // near 260rpm

    OC1CONbits.ON = 1;
    T2CONbits.ON = 1;
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
    U4STAbits.UTXEN = 0; // enable uart transmit
    U4MODEbits.ON = 1; // enable whole uart module
    // uart 4 error
    IEC2bits.U4EIE = 1; // error interrupt enabled


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

    //dma 1 for U6TX debugging
    DCH1CONbits.CHPRI = 2; // channel priority 2
    DCH1ECONbits.CHSIRQ = 72; // uart 6 tx Interrupt "IRQ" Vector Number (p.134)
    DCH1ECONbits.SIRQEN = 1; // enable cell transfer when IRQ triggered
    DCH1INT = 0; // clear all existing flags, disable all interrupts
    DCH1SSA = (unsigned int) &dma_one_array & 0x1FFFFFFF; // physical address conversion for transmit buffer
    DCH1DSA = (unsigned int) &U6TXREG & 0x1FFFFFFF; // physical address conversion for uart send buffer
//    DCH1SSIZ=3000; // source size at most 3000 bytes
    DCH1DSIZ = 1; // dst size is 1 byte
    DCH1CSIZ = 1; // one byte per UART transfer request
    
    //DMA 1 settings
    arrayOFdmaSetting[1].dma_array = (unsigned char*) &dma_one_array;
    arrayOFdmaSetting[1].dmacon = &DCH1CON;
    arrayOFdmaSetting[1].con_busy_mask = _DCH1CON_CHBUSY_MASK;
    arrayOFdmaSetting[1].con_en_mask = _DCH1CON_CHEN_MASK;
    arrayOFdmaSetting[1].dmasize = &DCH1SSIZ;
    arrayOFdmaSetting[1].dmaecon = &DCH1ECON;
    arrayOFdmaSetting[1].econ_force_mask = _DCH1ECON_CFORCE_MASK;

    //enable the interrupt for the dma 1 after each cell transfer
    IFS1bits.DMA1IF = 0;
    IPC9bits.DMA1IP = 2;
    IPC9bits.DMA1IS = 2; 
    IEC1bits.DMA1IE = 1;
    
    //After loading the DMA settings, start using the DMA
    queue_begin(arrayOFdmaSetting, 1);

}