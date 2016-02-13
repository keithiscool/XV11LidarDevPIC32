#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <psystem.h>
#include "libCoreTimer.h"
#include "apiHART.h"
#include <plib.h>
#include "libHartUART.h"

void EnByteCoreTimer(unsigned int Mode);
void DsByteCoreTimer(void);
void GetUSDly(unsigned int Dly);

void EnByteCoreTimer(unsigned int Mode)
{
    if(Mode == RTS_MODE)
    {
        OpenCoreTimer(RTS_CORE_TICK_RATE);
    }
    else
    {
        OpenCoreTimer(BYTE_CORE_TICK_RATE);
    }

    // set up the core timer interrupt with a prioirty of 2 and zero sub-priority
    mConfigIntCoreTimer((CT_INT_ON | CT_INT_PRIOR_2 | CT_INT_SUB_PRIOR_0));

    // enable multi-vector interrupts
    INTEnableSystemMultiVectoredInt();
}

void DsByteCoreTimer(void)
{
    CloseCoreTimer();
}

void __ISR(_CORE_TIMER_VECTOR, IPL2SOFT) CoreTimerHandler(void)
{
    // clear the interrupt flag
    mCTClearIntFlag();
    // update the period

    #ifdef EN_HART_STACK
    if(HART_STATE == HART_WAIT_RTS_DLY)
    {
        HART_STATE = nHART_STATE;
        DsByteCoreTimer();
    }
    else
    {
        DsByteCoreTimer();
        EnByteCoreTimer(BYTE_MODE);
    }
    #endif
}