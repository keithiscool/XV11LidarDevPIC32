#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>
#include "interrupt_handler.h"


bool timeFlagOneHundMilSec = false;
bool timeFlagFiveSec = false;


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

    //5000ms has passed
    if( (timeFlagFiveSec == false) && (count > 50) ) //5000ms has passed
    {
        count = 0; //reset timer finished counter
        timeFlagFiveSec = true; //set global variable for use with rest of code outside this interrupt (timer done)
    }

    //100ms has passed
    if( (timeFlagOneHundMilSec == false) && (count > 1) ) //100ms has passed
    {
        timeFlagOneHundMilSec = true; //set global variable for use with rest of code outside this interrupt (timer done)
    }
    

    IFS0CLR = _IFS0_T3IF_MASK;
}
