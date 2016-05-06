#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>

bool timeFlag = false;


//100ms timer (PR = 31250, prescaler = 256)
void __ISR(_TIMER_1_VECTOR, IPL1AUTO) Timer1Handler(void)
{
    static unsigned int count = 0;

    if(count++ > 1) //100ms has passed
    {
        count = 0; //reset timer finished counter
        timeFlag = true; //set global variable for use with rest of code outside this interrupt (timer done)
    }

    IFS0CLR = _IFS0_T1IF_MASK;
}
