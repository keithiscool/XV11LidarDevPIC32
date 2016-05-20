#include <xc.h>
#include <sys/attribs.h>
#include <stdbool.h>

     bool timeFlag = false;
     bool ms200done = false;
    
//100ms timer (PR = 31250, prescaler = 256)
void __ISR(_TIMER_3_VECTOR, IPL1AUTO) Timer3Handler(void)
{
    static unsigned int count = 0;

    static unsigned char wiiCount = 0;
    count++;
    wiiCount++;  

    if(ms200done == false && wiiCount>2)
    {
        wiiCount = 0;
        ms200done = true;
    }
    IFS0CLR = _IFS0_T3IF_MASK;
}




