/* 
 * File:   main.c
 * Author: Phoenix_PV_FRAME Primary_Var;
 *
 * Created on August 25, 2013, 12:58 PM
 * Modified on Septmber 16, 2013, 6:05 PM
 */
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pUART.h>
#include <pSystem.h>
#include <apiTick.h>

#ifdef __DEBUG
#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPBDIV = DIV_1,FCKSM = CSECME, FWDTEN = OFF
#pragma config OSCIOFNC = ON, POSCMOD = HS, FSOSCEN = ON, FNOSC = PRIPLL, WDTPS = PS32768
#pragma config CP = OFF, BWP = OFF, PWP = OFF
#pragma config ICESEL   = ICS_PGx1
#else
#pragma config FPLLODIV = DIV_1, FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPBDIV = DIV_1,FCKSM = CSECME, FWDTEN = ON
#pragma config OSCIOFNC = ON, POSCMOD = HS, FSOSCEN = ON, FNOSC = PRIPLL, WDTPS = PS32768
#pragma config CP = ON, BWP = OFF, PWP = OFF
#pragma config ICESEL   = ICS_PGx1
#endif

void ConfigSYSTEM(void);
void ConfigOSC(void);

int main(int argc, char** argv)
{
    static DWORD beatTick = 0;
    
    DelayMs(10);
    ConfigOSC();
    DelayMs(5);
    ConfigSYSTEM();
    ConfigTick();
    ConfigUART();
    DelayMs(100);    

    while(1)
    {
        ClrWdt();

        if(TickGet() - beatTick >= TICK_SECOND/2ul)
        {
            beatTick = TickGet();
            PutStrUART("Testing UART");
            BEAT_LED ^= 1;
        }        
    }
    
    return (EXIT_SUCCESS);
}

void ConfigSYSTEM(void)
{
    DDPCONbits.JTAGEN = 0;
    mPORTEClearBits(BIT_2);
    mPORTESetPinsDigitalOut(BIT_2);
    INTEnableSystemMultiVectoredInt();
 }

void ConfigOSC(void)
{
    OSCCON = 0b000100110100000;
    SYSTEMConfigPerformance(GetSystemClock());
    mOSCSetPBDIV(OSC_PB_DIV_1);
}




