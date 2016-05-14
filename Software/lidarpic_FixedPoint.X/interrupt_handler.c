#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>
#include "interrupt_handler.h"

bool timeFlag = false;


////100ms timer (PR = 31250, prescaler = 256)
//void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3Handler(void)
//{
//    static unsigned int count = 0;
//    count++;
//
////    if(count > 1) //100ms has passed
//    if( (timeFlag == false) && (count > 5) ) //100ms has passed
//    {
//        count = 0; //reset timer finished counter
//        timeFlag = true; //set global variable for use with rest of code outside this interrupt (timer done)
//    }
//
//    IFS0CLR = _IFS0_T3IF_MASK;
//}


//100ms timer (PR = 31250, prescaler = 256)
void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3Handler(void)
{
    static unsigned int count = 0;
    count++;

    if(count > 100)
        count--;

//    if(count > 1) //100ms has passed
    if( (timeFlag == false) && (count > 5) ) //100ms has passed
    {
        count = 0; //reset timer finished counter
        timeFlag = true; //set global variable for use with rest of code outside this interrupt (timer done)
    }
    
//        LATBbits.LATB9 ^= 1; //toggle on LED
//        LATBbits.LATB10 ^= 1; //toggle on LED
//        LATBbits.LATB11 ^= 1; //toggle on LED
//        LATBbits.LATB12 ^= 1; //toggle on LED
//        LATBbits.LATB13 ^= 1; //toggle on LED
//
//        LATBbits.LATB4 ^= 1;
//        LATFbits.LATF4 ^= 1;
//        LATDbits.LATD2 ^= 1;
//        LATDbits.LATD1 ^= 1;


    IFS0CLR = _IFS0_T3IF_MASK;
}
