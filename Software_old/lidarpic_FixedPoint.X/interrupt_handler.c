#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>
#include "interrupt_handler.h"


bool timeOneHundMilSec = false;
bool timeFlagOneHundMilSec = false;
bool timeFlagFiveSec = false;



//100ms timer (PR = 31250, prescaler = 256)
void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3Handler(void)
{

    //100ms has passed
    if(timeOneHundMilSec < 100000000) {
        timeOneHundMilSec++; //set global variable for use with rest of code outside this interrupt (timer done)
        timeFlagOneHundMilSec = true; //set global variable for use with rest of code outside this interrupt (timer done)
    }else{
        timeOneHundMilSec = 0;
    }

    //5000ms have passed
    if(timeOneHundMilSec > 50) {   //5000ms has passed
        timeFlagFiveSec = true; //set global variable for use with rest of code outside this interrupt (timer done)
    }

    IFS0CLR = _IFS0_T3IF_MASK;
}
