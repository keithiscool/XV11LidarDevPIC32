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


//  LCD Stuff:
//void DrawDistanceMap(void) {
//	// UP TO 3500 i distance = 3.5m
//    short deg;
//    static short LCDDisplayX[360], LCDDisplayY[360];
//    for (deg = 0; deg < 360; deg++) {
//        LCD_SetTextColor(LCD_COLOR_BLACK);
//        LCD_DrawLine(x[deg], y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//        LCD_DrawLine(x[deg]+1, y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//        LCD_DrawLine(x[deg], y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//        LCD_DrawLine(x[deg]+1, y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//
//        if((Distance[deg] < 3000) && (Distance[deg] > 0)) {
////            displayDistance = Distance[deg] / 25.0f;
////            x[deg] = (cos(M_PI_2-radians) * displayDistance) + 120;
////            y[deg] = (sin(M_PI_2-radians) * displayDistance) + 120;
//            LCD_SetTextColor(LCD_COLOR_WHITE);
//            LCDDisplayX = (XCoordMeters[deg] >> LCD_SCALING) + 120;
//            LCDDisplayY = (YCoordMeters[deg] >> LCD_SCALING) + 120;
//
//            LCD_DrawLine(x[deg], y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//            LCD_DrawLine(x[deg]+1, y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//            LCD_DrawLine(x[deg], y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//            LCD_DrawLine(x[deg]+1, y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
//        }
//    }
//    LCD_SetTextColor(LCD_COLOR_BLUE2);
//    LCD_DrawFullCircle(120, 120, 6);
//}


void delay(void){
    for(v=0; v<6000; v++);
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
    if(AnglesCoveredTotal >=359) { // all angles have data in them
        return 1;
    }else { //not all angles have distance data in them
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
    printf("PIC_RST\r\n");
    short DistanceDifferencesArr[360]; // used within objectDetection() function to find objects
    short MaximumObjectsToTrack = 100; //only track 100 objects within the arena dimensions
    short TrackedObjects[360];

    for(i=0;i<360;i++) {
        Distance[i] = 1; //Initialize all distances to '1' so I can see which ones are not getting written to
    }

    while (1){
        U5STAbits.URXEN = 1; // enable uart recieve
        IEC2bits.U5RXIE = 1; // enable uart recieve
        U1STAbits.UTXEN = 0; // disable uart transmit
        IEC0bits.U1TXIE = 0; // transmit interrupt disable (used in code as needed to permit TXdata from pic32 to PC)

////////        if (AllMeasurementsTaken() == 1) {
        if(LIDARdecode()==1) {
            LATEbits.LATE4 ^= 0; //Toggle LED1 on,off,on,off

            if(AnglesCoveredTotal >= 180) {
                //Show first index as zero
                printf("ChkPass \r\n",0); //designates to the terminal output that a the lidar checksum passed
                IEC0bits.U1TXIE = 1; // enable TX1 interrupt so the pic starts sending debug data
                U1STAbits.UTXEN = 1; // enable uart transmit

                if(operationMode==TESTMODE) {
                    U5STAbits.URXEN = 0; // disable uart receive (Do not allow Receive Data from Lidar UART5)
                    U5MODEbits.ON = 0; // disable whole uart5 module
                    printf("\x1b[HDisplayPolarData\r\n"); // ANSI Terminal code: ("ESC[H" == home) & ("ESC[2J" == clear screen)
                    for(i=0;i<360;i++) {
                        if(U1STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART1
                            if ((i % 16) == 0) { //print 16 distances per line
                                printf("\r\n%4u: ",i); //print the line degree number then print 16 distances after it '\r\n' causes prompt to carraige return
                            }
                            printf("%4u ",Distance[i]); //Print out the data to 4 unsigned (short) integers, (short == 16-bit unsigned integer for pic32/xc32 compiler)

                        } else {
                            i--; //keep index at same value if the UART1 TX debug buffer is full
                        }
//                        printf("\x1b[0J"); // ANSI Terminal code: ("ESC[2J" == clear screen below cursor)
//                        if((objectDetection(&DistanceArr[360], &DistanceDifferencesArr[360]), TrackedObjects[360]) <= MaximumObjectsToTrack) {
                        if( (objectDetection(i, &Distance, &DistanceDifferencesArr, &TrackedObjects)) <= MaximumObjectsToTrack) {


                        }

                    }

////////
////////                    printf("DisplayCartesianData\r\n");
////////                    printf("XCoordMeters , YCoordMeters:\r\n");
////////                    for(i=0;i<360;i++) {
//////////                        while(U1STAbits.TRMT == 1) { //check to see if the UART buffer is empty - if it is, send debug data out UART1
////////                        if(U1STAbits.UTXBF == 0) { //check to see if the UART buffer is not full - if it is not full, send debug data out UART1
////////                            if ((i % 4) == 0)  //print 4 x,y distances per line
////////                                printf("\r\n%4d: ",i); //print 4 distances per line '\r\n' causes prompt to carraige return
////////
////////                            printf(" %d,%d//", XCoordMeters[i], YCoordMeters[i]);
////////                        }
////////                        else
////////                            i--; //keep index at same value if the UART1 TX debug buffer is full
////////                    }
////////                    printf("\r\n--------------------------------\r\n");
                }
                U5STAbits.URXEN = 1; // enable uart transmit (Allow Receive Data from Lidar)
                IEC0bits.U1TXIE = 0; // disable TX1 interrupt so the pic stops sending debug data
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