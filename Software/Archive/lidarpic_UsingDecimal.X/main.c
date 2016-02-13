 /* File:   main.c
 * Author: Igor
 * Created on July 3, 2015, 1:11 PM
 */
#include <xc.h>
#include <stdbool.h>
#include <stdio.h>
#include "main.h"
#include <math.h>
#include <string.h>


// PIC32MX575F512H Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // SRS Select (SRS Priority 7)
#pragma config FCANIO = OFF             // CAN I/O Pin Select (Alternate CAN I/O)
#pragma config FUSBIDIO = OFF           // USB USID Selection (Controlled by Port Function)
#pragma config FVBUSONIO = OFF          // USB VBUS ON Selection (Controlled by Port Function)

// DEVCFG2
#pragma config FPLLIDIV = DIV_6         // PLL Input Divider (6x Divider)
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


void delay(void){
    for(v=0; v<60000; v++);
}

int AllMeasurementsTaken(void){
    int j = 0;
    for(j=0;j<90;j++) {
        LIDARdecode();
    }

    //Verify that all 360 degrees have a distance measurement
    for(i=0;i<360;i++) {
        AnglesCoveredTotal += SuccessfulMeasurements[i];
    }
    
    if(AnglesCoveredTotal >=359) {
        return 1;
    }else {
        AnglesCoveredTotal = 0;
        return 0;
    }
    
    return 0;
}

void main(void){
    TRISEbits.TRISE4 = 0;
    TRISEbits.TRISE3 = 0;
    LATEbits.LATE3 = 1;
    initialize();
    printf("Initialize_Power_Up_PIC\r\n");

    //Initialize all distances to '99' so I can see which ones are not getting written to
    for(i=0;i<360;i++) {
        Distance[i] = 1111;
    }

    while (1){
////////        if (AllMeasurementsTaken() == 1) {
        if(LIDARdecode()==1) {
            LATEbits.LATE4 ^= 0; //Toggle LED1 on,off,on,off

            if(AnglesCoveredTotal >= 180) { //Make sure at least 180 distance measurements have been taken

                if(operationMode==TESTMODE) {
                    U5STAbits.URXEN = 0; // disable uart receive (Do not allow Receive Data from Lidar UART5)
                    U5MODEbits.ON = 0; // disable whole uart5 module
                    printf("Deg:    DisplayPolarData\r\n");
                    for(i=0;i<360;i++) {

                        if(U1STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART1
//                            if ((i % 8) == 0) //print 8 distances per line
                                printf("\r\n%4d ",i); //print 8 distances per line '\r\n' causes prompt to carraige return

                            printf("%4u ",Distance[i]); //Print out the data to 4, 16-bit unsigned integer digits
                        }
                        else
                            i--; //keep index at same value if the UART1 TX debug buffer is full
                    }

                    printf("\r\n");
                    printf("\r\n--------------------------------\r\n");
                    printf("DisplayCartesianData\r\n");
                    printf("XCoordMeters , YCoordMeters:\r\n");
                    for(i=0;i<360;i++) {
                        if(U1STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART1
//                            if ((i % 4) == 0)  //print 8 distances per line
                                printf("\r\n%4d ",i); //print 8 distances per line '\r\n' causes prompt to carraige-return and text prints on new line

                            printf(" %d  %d", XCoordMeters[i], YCoordMeters[i]); //Print the cartesian distance data
                        }
                        else 
                            i--; //keep index at same value if the UART1 TX debug buffer is full
                    }
                    printf("\r\n--------------------------------\r\n");
                }
                
            U5STAbits.URXEN = 1; // enable uart transmit (Allow Receive Data from Lidar)
            U5MODEbits.ON = 1; // enable whole uart module
            }
        }
    }
}


static enum
{
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
    LATEbits.LATE3 = 0; //test 2
    while (1)
    {
        // Examine _excep_code to identify the type of exception
        // Examine _excep_addr to find the address that caused the exception
    }
}