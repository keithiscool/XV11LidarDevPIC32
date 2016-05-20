#include <xc.h>
#define _SUPPRESS_PLIB_WARNING
#include <plib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "main.h"
#include "wiiCamera.h"
#include "Servos.h"
#include "SinLookupTable.h"
#include "uart_handler.h"


#define CAMERA1 0
#define CAMERA2 1
#define COLLECTOR_BIN 150 // distance apart the wii cameras are in cm?
#define CAMERA_ADDRESS 3
#define LANTRONIX   4
#define DEGREE_TO_RADIAN  0.0174532925194329577

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FCANIO = OFF             // CAN I/O Pin Select (Alternate CAN I/O)
#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_6         // PLL Input Divider (6x Divider) (Phase Lock Loop- stepping up the frequency)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (20x Multiplier)
#pragma config UPLLIDIV = DIV_6         // USB PLL Input Divider (6x Divider)
#pragma config UPLLEN = ON              // USB PLL Enable (Enabled)
#pragma config FPLLODIV = DIV_1         // System PLL Output Clock Divider (PLL Divide by 1)

// DEVCFG1
#pragma config FNOSC = PRIPLL           // Oscillator Selection Bits (Primary Osc w/PLL (XT+,HS+,EC+PLL))
#pragma config FSOSCEN = OFF            // Secondary Oscillator Enable (Disabled)
#pragma config IESO = OFF               // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = HS             // Primary Oscillator Configuration (HS osc mode)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_1           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/1)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (WDT Disabled (SWDTEN Bit Controls))

// DEVCFG0
#pragma config DEBUG = ON               // Background Debugger Enable (Debugger is enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (ICE EMUC1/EMUD1 pins shared with PGC1/PGD1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

#define SYS_FREQ 80000000 // system frequency

bool awaitingData = false;
bool dataAvailable = false;

int receiveArray[20];

void timers(void){
    // timer for servo 1
    T4CONbits.ON = 0;
    T4CONbits.TCKPS = 0b101; // 32 prescalar
    T4CONbits.TGATE = 0;
    T4CONbits.T32 = 0;
    IFS0CLR = _IFS0_T4IF_MASK;
    IEC0SET = _IEC0_T4IE_MASK;
    IPC4SET = ((0x111 << _IPC4_T4IP_POSITION) | (0x1 << _IPC4_T4IS_POSITION));
    T4CONbits.ON = 1;

    //timer for servo 2
    T5CONbits.ON = 0;
    T5CONbits.TCKPS = 0b101; // 32 prescalar
    T5CONbits.TGATE = 0;
    IFS0CLR = _IFS0_T5IF_MASK;
    IEC0SET = _IEC0_T5IE_MASK;
    IPC5SET = ((0x111 << _IPC5_T5IP_POSITION) | (0x1 << _IPC5_T5IS_POSITION)); // What does this do?

    T5CONbits.ON = 1;

    // general timing timer for everything
    T3CONbits.ON = 0;
    T3CONbits.TCKPS = 0b111; //256 prescalar
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
void initializeBits(void)
{
     SYSTEMConfig(SYS_FREQ, SYS_CFG_ALL); //memory management
    //INTEnableSystemMultiVectoredInt(); //enables interrupts

    DDPCONbits.JTAGEN = 0; // This turns off the JTAG and allows PORTA pins to be used (Must disable the JTAG module in order to use LAT registers to set discrete outputs (LEDs)

    // TRIS ? Set a pin as Input or Output
    // LAT ? output
    // PORT ? inpput
    //LED
    TRISBbits.TRISB9 = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;
    TRISBbits.TRISB12 = 0;
    TRISBbits.TRISB13 = 0;

    LATBbits.LATB9 = 1;
    LATBbits.LATB10 = 1;
    LATBbits.LATB11 = 1;
    LATBbits.LATB12 = 1;
    LATBbits.LATB13 = 1;

    I2C3BRG = 390;
    IPC6bits.I2C3IP = 1; //priority 1
    IPC6bits.SPI1AIS = 2; //priority 2
    IEC0bits.I2C3MIE = 1; //enable I2C interrupt
    I2C3CONbits.ON = 1; //enable I2C

    I2C5BRG = 390;
    IPC8bits.I2C5IP = 1; //priority 1
    IPC8bits.SPI3AIS = 2; //priority 2
    IEC1bits.I2C5MIE = 1; //enable I2C interrupt
    I2C5CONbits.ON = 1; //enable I2C

   
    UART();
    begin(&receiveArray, sizeof(receiveArray),CAMERA_ADDRESS,false, Send_put,Receive_get,Receive_available,Receive_peek);
    
    timers();

    INTEnableSystemMultiVectoredInt(); //enables interrupts

    INTEnableInterrupts(); // enable interrupts
}

int CalculateHeight(unsigned char state)
{
    // if don't see both blobs
    if(state < 6)
        return 0;
    // if see both blobs (either one each for the servos, or both servos see both)
    return COLLECTOR_BIN * sin(getAngle1()*DEGREE_TO_RADIAN)*sin(getAngle2()*DEGREE_TO_RADIAN)/sin((180-getAngle1()-getAngle2())*DEGREE_TO_RADIAN);
}
int offCenter(unsigned char state)
{
    // if don't see both blobs
    if(state < 6)
        return 0;
    // if see both blobs (either one each for the servos, or both servos see both)
    return COLLECTOR_BIN/2 - COLLECTOR_BIN*sin(getAngle2()*DEGREE_TO_RADIAN)*cos(getAngle1()*DEGREE_TO_RADIAN)/ sin((180-getAngle1()- getAngle2())*DEGREE_TO_RADIAN);
}

unsigned char determineState()
{
    bool goodr1 = false;
    bool goodr2 = false;
    if(result1<1023)
    {
        goodr1 = true;
    }
    if(result2<1023)
    {
        goodr2 = true;
    }
    if(goodr1 && goodr2){ // good r values from the blobs
    if(getBlobsSeen(CAMERA1)<=0 && getBlobsSeen(CAMERA2)<=0)// can't see any blobs
        return 0;
    if((getBlobsSeen(CAMERA1)>1 && getBlobsSeen(CAMERA2)>1))// both see both blobs
        return 7;
    if(getBlobsSeen(CAMERA1)==1 && getBlobsSeen(CAMERA2)<=0)//camera1 sees one blob only
        return 1;
    if(getBlobsSeen(CAMERA1)<=0 && getBlobsSeen(CAMERA2)==1) // camera2 sees one blob only
        return 2;
    if(getBlobsSeen(CAMERA1)>1 && getBlobsSeen(CAMERA2)<=0) // camera1 sees both blobs only
        return 3;
    if(getBlobsSeen(CAMERA1)<=0 && getBlobsSeen(CAMERA2)>1) // camera2 sees both blobs only
       return 4;
    if(getBlobsSeen(CAMERA1)==1 && getBlobsSeen(CAMERA2)==1) // both cameras see one blob
    {
        if((180-getAngle1()-getAngle2())<0) //cameras each see different blobs
            return 5;
     return 6; // cameras see same blob
    }
    }
    else if(goodr1&&!goodr2) // no good blobs from camera2
    {
      if(getBlobsSeen(CAMERA1)<=0)// can't see any blobs camera1
        return 0;
      if(getBlobsSeen(CAMERA1)==1)//camera1 sees one blob only
        return 1;
      if(getBlobsSeen(CAMERA1)>1) // camera1 sees both blobs only
        return 3;
    }
    else if (!goodr1&& goodr2)// no good blobs from camera1
    {
      if(getBlobsSeen(CAMERA2)<=0)// can't see any blobs camera2
        return 0;
      if(getBlobsSeen(CAMERA2)==1) // camera2 sees one blob only
        return 2;
      if(getBlobsSeen(CAMERA2)>1) // camera2 sees both blobs only
       return 4;
    }
    else
    {
        return 0;
    }

}

void main(void) {
   
    initializeBits();
    while(wiiInitStep<=5){

    initializeWii();
  
    }
//timers();
        //delay
    int i = 0;
    while (i < 100000) {

        i++;
    }

       
    while (1) {

    
        if (ms200done && !awaitingData) {//Do I need to wait until not dataAvailable as well?

        //U6TXREG=0x99;
        unsigned char state = determineState();
        ToSend(0,CAMERA_ADDRESS);
        ToSend(9,state);
        ToSend(10,getAngle1());//left camera when facing, lidar side
        ToSend(11,getAngle2());//right camera
        ToSend(12,offCenter(state));
        ToSend(13,CalculateHeight(state));
        ToSend(14,getDifferenceX(CAMERA1));//left camera, lidar side
        ToSend(15,getDifferenceX(CAMERA2)); // right camera
     
        sendData(LANTRONIX);


            LATBbits.LATB11 ^= 1;
            read(CAMERA1);
            read(CAMERA2);
            ms200done = false;
            awaitingData = true;
        }
         if (awaitingData) {
            if (StatusI2Cone() == 1 && StatusI2Ctwo()==1) {
                dataAvailable = true;
                awaitingData = false;
            }
            else if(StatusI2Cone() == 2 || StatusI2Ctwo()==2)
            {
                awaitingData=false;
            }
        }
        if (dataAvailable) {

            dataAvailable = false;
            Sort(CAMERA1);
            Sort(CAMERA2);
            result1 = PickBeacon(CAMERA1);
            result2 = PickBeacon(CAMERA2);
            //camera and servo1
            //seekBeacon(CAMERA1, result1);
            //seekBeacon(CAMERA2, result2);
            //printf("Hi");
//            if (getBlobsSeen(CAMERA1) > 0 && result1 < 1023) {
//                determineAngle(true, result1, CAMERA1);
//            } else {
//                determineAngle(false, result1, CAMERA1);
//
//            }
//            // camera and servo 2
//            if (getBlobsSeen(CAMERA2) > 0 && result2 <= 1023) {
//                determineAngle(true, result2, CAMERA2);
//            } else {
//                determineAngle(false, result2, CAMERA2);
//            }
//            static int counter =0;
//            if( counter>2)
//            {
            seekBeacon(CAMERA1, result1);
            seekBeacon(CAMERA2, result2);
//                counter = 0;
//            }
//            counter++;
        }

     



         }
    
}

static enum {
    EXCEP_IRQ = 0, // interrupt
    EXCEP_AdEL = 4, // address error exception (load or ifetch)
    EXCEP_AdES, // address error exception (store)
    EXCEP_IBE, // bus error (ifetch)
    EXCEP_DBE, // bus error (load/store)
    EXCEP_Sys, // syscall
    EXCEP_Bp, // breakpoint
    EXCEP_RI, // reserved instruction
    EXCEP_CpU, // coprocessor unusable
    EXCEP_Overflow, // arithmetic overflow
    EXCEP_Trap, // trap (possible divide by zero)
    EXCEP_IS1 = 16, // implementation specfic 1
    EXCEP_CEU, // CorExtend Unuseable
    EXCEP_C2E // coprocessor 2
} _excep_code;
static unsigned int _epc_code;
static unsigned int _excep_addr;
// this function overrides the normal _weak_ generic handler

void _general_exception_handler(void) {
    asm volatile("mfc0 %0,$13" : "=r" (_excep_code));
    asm volatile("mfc0 %0,$14" : "=r" (_excep_addr));
    _excep_code = (_excep_code & 0x0000007C) >> 2;
    LATBbits.LATB10 = 0; //test 1
    while (1) {

        // Examine _excep_code to identify the type of exception
        // Examine _excep_addr to find the address that caused the exception
    }
}
