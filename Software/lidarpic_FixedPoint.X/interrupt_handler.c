
#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>

bool timeFlag = false;


void __ISR(_TIMER_1_VECTOR, IPL1AUTO) Timer1Handler(void)
{
    static unsigned int time = 0;

    if(time++ > 500)
    {
        time = 0;
        timeFlag = true;
    }



    IFS0CLR = _IFS0_T1IF_MASK;
}
