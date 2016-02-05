/*
 * File:   psystem.h
 * Author: Phoenix
 *
 * Created on July 22, 2013, 7:26 PM
 */

#ifndef PSYSTEM_H
#define	PSYSTEM_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <apiDelay.h>
#include <GenericTypeDefs.h>
#include <Compiler.h>

/*********************************************************************
* Macro: #define	GetSystemClock()
*
* Overview: This macro returns the system clock frequency in Hertz.
*			* value is 8 MHz x 4 PLL for PIC24
*			* value is 8 MHz/2 x 18 PLL for PIC32
*
********************************************************************/
#if defined(__PIC24F__)
    #define GetSystemClock()    (32000000ul)
#elif defined(__PIC32MX__)
    #define GetSystemClock()    (80000000ul)
#elif defined(__dsPIC33F__) || defined(__PIC24H__)
    #define GetSystemClock()    (80000000ul)
#endif

/*********************************************************************
* Macro: #define	GetPeripheralClock()
*
* Overview: This macro returns the peripheral clock frequency
*			used in Hertz.
*			* value for PIC24 is <PRE>(GetSystemClock()/2) </PRE>
*			* value for PIC32 is <PRE>(GetSystemClock()/(1<<OSCCONbits.PBDIV)) </PRE>
*
********************************************************************/
#if defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__)
    #define GetPeripheralClock()    (GetSystemClock() / 2)
#elif defined(__PIC32MX__)
    #define GetPeripheralClock()    (GetSystemClock()/1)    //(GetSystemClock() / (1 << OSCCONbits.PBDIV))
#endif

/*********************************************************************
* Macro: #define	GetInstructionClock()
*
* Overview: This macro returns instruction clock frequency
*			used in Hertz.
*			* value for PIC24 is <PRE>(GetSystemClock()/2) </PRE>
*			* value for PIC32 is <PRE>(GetSystemClock()) </PRE>
*
********************************************************************/
#if defined(__PIC24F__) || defined(__PIC24H__) || defined(__dsPIC33F__)
    #define GetInstructionClock()   (GetSystemClock() / 2)
#elif defined(__PIC32MX__)
   #define GetInstructionClock()   (GetSystemClock())
#endif

#if defined (USE_MULTIBYTECHAR)
    #define XCHAR   unsigned short
    #define UXCHAR  unsigned short
#elif defined (USE_UNSIGNED_XCHAR)
    #define XCHAR   unsigned char
    #define UXCHAR  unsigned char
#else
    #define XCHAR   char
    #define UXCHAR  unsigned char
#endif

#define uUART_TX_BUFFER_SIZE       750
    
#define ASCII_BASE_VAL  0x30

#define PASS    0
#define FAIL    1

#define LED_PUT(a)		do{LATD = (LATD & 0xFFF8) | ((a)&0x07);}while(0)
#define BEAT_LED_TRIS		(TRISEbits.TRISE2)	// Ref LED1
#define BEAT_LED		(LATEbits.LATE2)

#define OFF     0

#ifdef	__cplusplus
}
#endif

#endif	/* PSYSTEM_H */

