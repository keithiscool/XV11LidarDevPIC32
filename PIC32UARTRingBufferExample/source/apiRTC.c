#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "apiRTC.h"
#include "pSystem.h"
#include "apiEEPROM.h"

/******************************** 		*********************************	*********************************		 *********************************
********************************* 		*********************************	*********************************		 *********************************
				Demo On Uart
					Function 					Use
				1. RTCInit				It intialize the RTC
********************************** 		*********************************	*********************************		 *********************************
********************************** 		*********************************	*********************************		 *********************************/

#if defined I2C
void ConfigI2C(void);
#endif

#if defined RTC
void RTCInit(void);

void ConfigRTC(void);
void SetALMTime(void);
unsigned char ReadSecond(void);
void GetRTCString(char* pcString);
void GetTimeString(char* pcString);
void RdRTC(_DATETIME *ptrDateTime);
static unsigned int uint2bcd(unsigned int ival);
void SetRtcTime(unsigned char Seconds,unsigned char Minutes,unsigned char Hours);
void SetRtcDate(unsigned char Date,unsigned char Month,unsigned char Year);

TIMESTAMP RTS, WTS;
char TimeFrame[10];
char RtcFrame[20];
#endif

#if defined I2C
/***********************************************************************************************************************
										BASIC I2C CODES FOR WRITE AND READ CYCLE
************************************************************************************************************************/
BOOL StartTransfer(BOOL restart)
{
    if(restart)
    {
        I2CRepeatStart(I2C2);
    }
    else
    {
        while( !I2CBusIsIdle(I2C2));
        if(I2CStart(I2C2) != I2C_SUCCESS)
        {
            return FALSE;
        }
    }

    do
    {
        status = I2CGetStatus(I2C2);
    } while (!(status & I2C_START));

    return TRUE;
}
/*******************************************************************************************************************************************
********************************************************************************************************************************************/
void StopTransfer(void)
{
    I2C_STATUS  status;
    I2CStop(I2C2);

    do
    {
        status = I2CGetStatus(I2C2);
    }while (!(status & I2C_STOP));
}
/*******************************************************************************************************************************************
********************************************************************************************************************************************/
BOOL TransmitOneByte(UINT8 data)
{
    while(!I2CTransmitterIsReady(I2C2));

    if(I2CSendByte(I2C2, data) == I2C_MASTER_BUS_COLLISION)
    {
        return FALSE;
    }

    while(!I2CTransmissionHasCompleted(I2C2));
    return TRUE;
}
/*******************************************************************************************************************************************
********************************************************************************************************************************************/
void WriteOnI2C(unsigned char RegAddr , unsigned char Value)
{

    int     Index =0;
    int     DataSz;
    BOOL    Success = TRUE;

    WriteAddress[1] = RegAddr;
    WriteAddress[2] = Value;
    DataSz = 3;

    if( !StartTransfer(FALSE))
    {
         while(1);
    }

    while(Success && (Index < DataSz))
    {
        if (TransmitOneByte(WriteAddress[Index]))
        {
            Index++;
            if(!I2CByteWasAcknowledged(I2C2))
            {
                Success = FALSE;	// Verify that the byte was acknowledged
            }
        }
        else
        {
            Success = FALSE;
        }
    }
    StopTransfer();
}
/*******************************************************************************************************************************************
********************************************************************************************************************************************/
unsigned char ReadOnI2C(unsigned char RegAddress)
{
    int     Index;
    int     DataSz;
    BOOL    Success = TRUE;

    WriteAddress[1] = RegAddress;
    DataSz = 2;

    if( !StartTransfer(FALSE))
    {
        while(1);
    }

    Index = 0;
    while( Success & (Index < DataSz))
    {
        if (TransmitOneByte(WriteAddress[Index]))
        {
            Index++;
        }
        else
        {
            Success = FALSE;
        }
        if(!I2CByteWasAcknowledged(I2C2))
        {
            Success = FALSE;
        }
    }

    if(Success)
    {
        if( !StartTransfer(TRUE))
        {						// Restart the I2C
			while(1);
        }

        if (TransmitOneByte(ReadAddress[0]))
        {			// Reading the Register value by Address.................
            if(!I2CByteWasAcknowledged(I2C2))
            {
                Success = FALSE;
            }
        }
        else
        {
            Success = FALSE;
        }
    }
    if(Success)
    {
        if(I2CReceiverEnable(I2C2, TRUE) == I2C_RECEIVE_OVERFLOW)
        {
            Success = FALSE;
        }
        else
        {
            while(!I2CReceivedDataIsAvailable(I2C2));
            i2cbyte = I2CGetByte(I2C2);
        }
    }
    StopTransfer();
    return i2cbyte;
}
#endif

#if defined RTC
/*******************************************************************************************************************************************
********************************************************************************************************************************************
												Set & Read RTC Time
********************************************************************************************************************************************
********************************************************************************************************************************************/

void SetRtcTime(unsigned char Seconds,unsigned char Minutes,unsigned char Hours)
{
    WTS.Second = (Seconds | 0x80);
    WTS.Minute = Minutes;
    WTS.Hour   = Hours;

    WriteAddress[0] = RTC_WRITE_ADDR;
    WriteSeconds();
    DelayMs(2);
    WriteMinutes();
    DelayMs(2);
    WriteHours();
    DelayMs(2);
}

void SetRtcDate(unsigned char Date,unsigned char Month,unsigned char Year)
{
    WTS.Date  = Date;
    WTS.Month = Month;
    WTS.Year  = Year;

    WriteAddress[0] = RTC_WRITE_ADDR;
    WriteDate();
    DelayMs(2);
    WriteDay();
    DelayMs(2);
    WriteMonth();
    DelayMs(2);
    WriteYear();
    DelayMs(2);
}

void WriteSeconds(void)
{
    WriteAddress[0] = RTC_WRITE_ADDR;
    WriteOnI2C(0x00,WTS.Second);
}

void WriteMinutes(void)
{
    WriteOnI2C(0x01,WTS.Minute);
}

void WriteHours(void)
{
    WriteOnI2C(0x02,WTS.Hour);
}

void WriteDay(void)
{
    WriteOnI2C(0x03,0x09);
}

void WriteDate(void)
{
    WriteOnI2C(0x04,WTS.Date);
}

void WriteMonth(void)
{
    WriteOnI2C(0x05,WTS.Month);
}

void WriteYear(void)
{
    WriteOnI2C(0x06,WTS.Year);
}

/*******************************************************************************************************************************************
********************************************************************************************************************************************/

void ReadRtcTime(unsigned char Seconds,unsigned char Minutes,unsigned char Hours)
{
    WriteAddress[0] = RTC_WRITE_ADDR;
    ReadAddress[0]  = RTC_READ_ADDR;
    ReadSeconds();
    ReadMinutes();
    ReadHours();
    RTS.Second  =  ((RSecond & 0x70) >> 4)*10  + (RSecond & 0x0F);
    RTS.Minute  =  ((RMinute & 0xF0) >> 4)*10  + (RMinute & 0x0F);
    RTS.Hour    =  ((RHour & 0xF0)   >> 4)*10  + (RHour & 0x0F);
    Seconds = RTS.Second;
    Minutes = RTS.Minute;
    Hours   = RTS.Hour;
}

void ReadRtcDate(unsigned char Date,unsigned char Month,unsigned char Year)
{
    ReadDay();
    ReadDate();
    ReadMonth();
    ReadYear();
    RTS.Year    =  ((RYear & 0xF0)   >> 4)*10  + (RYear & 0x0F);
    RTS.Date    =  ((RDate & 0x30)   >> 4)*10  + (RDate & 0x0F);
    RTS.Month   =  ((RMonth & 0x10)  >> 4)*10  + (RMonth & 0x0F);
    Date  = RTS.Date;
    Month = RTS.Month;
    Year  = RTS.Year;
}

void ReadSeconds(void)
{
    RSecond = ReadOnI2C(0x00);
    DelayMs(2);
    if((RSecond&0x80) == 0)
    {
        WriteSeconds();
    }
    DelayMs(2);
}

void ReadMinutes(void)
{
    RMinute = ReadOnI2C(0x01);
    DelayMs(2);
}

void ReadHours(void)
{
    RHour = ReadOnI2C(0x02);
    DelayMs(2);
}

void ReadDay(void)
{
    RDay = ReadOnI2C(0x03);
    if((RDay&0x08) == 0)
    {
        WriteDay();
    }
    DelayMs(2);
}

void ReadDate(void)
{
    RDate = ReadOnI2C(0x04);
    DelayMs(2);
}

void ReadMonth(void)
{
    RMonth = ReadOnI2C(0x05);
    DelayMs(2);
}

void ReadYear(void)
{
    RYear = ReadOnI2C(0x06);
    DelayMs(2);
}

unsigned char ReadSecond(void)
{
    unsigned char sec;

    WriteAddress[0] = RTC_WRITE_ADDR;
    ReadAddress[0]  = RTC_READ_ADDR;

    sec = ReadOnI2C(0x00);
    return sec;
}

void RdRTC(_DATETIME *ptrDateTime)
{
    int temp;

    WriteAddress[0] = RTC_WRITE_ADDR;
    ReadAddress[0]  = RTC_READ_ADDR;

    temp = ReadOnI2C(ADDR_SEC);
    (*ptrDateTime).SEC = 0;
    (*ptrDateTime).SEC = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_MIN);
    (*ptrDateTime).MIN = 0;
    (*ptrDateTime).MIN = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_HOUR);
    (*ptrDateTime).HOUR = 0;
    (*ptrDateTime).HOUR = (((temp >> 4) & 0x03) * 10) + (temp & 0x0F);
    (*ptrDateTime).AM_PM = 0;

    if((*ptrDateTime).HOUR >= 12)
            (*ptrDateTime).AM_PM = 1;

    DelayMs(2);

    temp = ReadOnI2C(ADDR_DATE);
    (*ptrDateTime).DATE = 0;
    (*ptrDateTime).DATE = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_MNTH);
    (*ptrDateTime).MONTH = 0;
    (*ptrDateTime).MONTH = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);

    DelayMs(2);

    temp = ReadOnI2C(ADDR_YEAR);
    (*ptrDateTime).YEAR = 0;
    (*ptrDateTime).YEAR = (((temp >> 4) & 0x07) * 10) + (temp & 0x0F);
}

void VBATInit(void)
{
    WriteAddress[0] = RTC_WRITE_ADDR;
    WriteOnI2C(ADDR_STAT, 0x08);
}

void RTCINTInit(void)
{
    PORTSetPinsDigitalIn(IOPORT_E, BIT_9);
    INTCONSET = 0x00000002;
    INTSetVectorPriority(INT_EXTERNAL_2_VECTOR, INT_PRIORITY_LEVEL_3);
    INTSetVectorSubPriority(INT_EXTERNAL_2_VECTOR, INT_SUB_PRIORITY_LEVEL_1);
    INTEnable(INT_INT2, INT_ENABLED);
}

void ConfigI2C(void)
{
    I2CConfigure(I2C2, I2C_ENABLE_SLAVE_CLOCK_STRETCHING);
    I2CSetFrequency(I2C2, GetSystemClock(), RTC_CLK);
    I2CSetSlaveAddress(I2C2,RTC_WRITE_ADDR, 0, I2C_USE_7BIT_ADDRESS);
    I2CEnable(I2C2, TRUE);
}

void ConfigRTC(void)
{
    unsigned sec1 = 0;
    unsigned sec2 = 0;

    sec1 = (int)ReadSecond();
    DelayMs(1200);
    sec2 = (int)ReadSecond();

    if(sec1 == sec2)
    {
        RTCInit();
    }

    VBATInit();
    DelayMs(2);

    #ifdef RTC_ALM
    SetALMTime();
    DelayMs(2);
    RTCINTInit();
    DelayMs(2);
    #endif
}

void RTCInit(void)
{
    char day;
    char month;
    char year;
    char hour;
    char min;
    char sec;

    sec =  (((__TIME__[6]) & 0x0F) << 4) | ((__TIME__[7]) & 0x0F);
    min =  (((__TIME__[3]) & 0x0F) << 4) | ((__TIME__[4]) & 0x0F);
    hour = (((__TIME__[0]) & 0x0F) << 4) | ((__TIME__[1]) & 0x0F);
    day =  (((__DATE__[4]) & 0x0F) << 4) | ((__DATE__[5]) & 0x0F);
    year = (((__DATE__[9]) & 0x0F) << 4) | ((__DATE__[10]) & 0x0F);

    switch(__DATE__[0])
    {
        case 'J':
            //January, June, or July
            switch(__DATE__[1])
            {
                case 'a':
                    //January
                    month = 0x01;
                    break;
                case 'u':
                    switch(__DATE__[2])
                    {
                        case 'n':
                            //June
                            month = 0x06;
                            break;
                        case 'l':
                            //July
                            month = 0x07;
                            break;
                    }
                    break;
            }
            break;
        case 'F':
            month = 0x02;
            break;
        case 'M':
            //March,May
            switch(__DATE__[2])
            {
                case 'r':
                    //March
                    month = 0x03;
                    break;
                case 'y':
                    //May
                    month = 0x05;
                    break;
            }
            break;
        case 'A':
            //April, August
            switch(__DATE__[1])
            {
                case 'p':
                    //April
                    month = 0x04;
                    break;
                case 'u':
                    //August
                    month = 0x08;
                    break;
            }
            break;
        case 'S':
            month = 0x09;
            break;
        case 'O':
            month = 0x10;
            break;
        case 'N':
            month = 0x11;
            break;
        case 'D':
            month = 0x12;
            break;
    }

    SetRtcTime(sec, min, hour);
    SetRtcDate(day, month, year);
}

static unsigned int uint2bcd(unsigned int ival)
{
    return ((ival / 10) << 4) | (ival % 10);
}

#ifdef RTC_ALM
void SetALMTime(void)
{
    int temp;

    WriteAddress[0] = RTC_WRITE_ADDR;

    temp = ReadOnI2C(ADDR_ALM0CTL);
    temp = temp & 0xF0;
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0CTL, temp);
    DelayMs(2);

    RdRTC(&AppStatus.DATE_TIME);
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0MTH, uint2bcd(AppStatus.DATE_TIME.MONTH));
    DelayMs(2);
    WriteOnI2C(ADDR_ALM0DAT, uint2bcd(AppStatus.DATE_TIME.DATE));
    DelayMs(2);

    temp = AppStatus.DATE_TIME.MIN + Task_Duration;

    if(temp >= 60)
    {
        temp -= 60;

        WriteOnI2C(ADDR_ALM0MIN, temp);
    	DelayMs(2);

    	if(AppStatus.DATE_TIME.HOUR == 23)
            WriteOnI2C(ADDR_ALM0HR, 0);
        else
            WriteOnI2C(ADDR_ALM0HR, uint2bcd(AppStatus.DATE_TIME.HOUR + 1));
    }
    else
    {
        WriteOnI2C(ADDR_ALM0MIN, uint2bcd(AppStatus.DATE_TIME.MIN + Task_Duration));
    	DelayMs(2);
    	WriteOnI2C(ADDR_ALM0HR, uint2bcd(AppStatus.DATE_TIME.HOUR));
    }

    DelayMs(2);

    WriteOnI2C(ADDR_ALM0SEC, uint2bcd(AppStatus.DATE_TIME.SEC));
    DelayMs(2);

    WriteOnI2C(ADDR_ALM0CTL, ALMx_POL + ALMxC_MIN + 0x01);

    DelayMs(2);
    WriteOnI2C(ADDR_CTRL, ALM_0);
    DelayMs(2);
}
#endif

void GetRTCString(char* pcString)
{
    //char RtcFrame[20];
    char tBuf[2];
    
    RdRTC(&AppStatus.DATE_TIME);
    sprintf(tBuf, "%d", AppStatus.DATE_TIME.DATE);

    if(tBuf[1] == 0)
    {
        RtcFrame[1] = tBuf[0];
        RtcFrame[0] = 0x30;
    }
    else
    {
        RtcFrame[0] = tBuf[0];
        RtcFrame[1] = tBuf[1];
    }

    RtcFrame[2]	= '/';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.MONTH);
    
    if(tBuf[1] == 0)
    {
        RtcFrame[4] = tBuf[0];
        RtcFrame[3] = 0x30;
    }
    else
    {
        RtcFrame[3] = tBuf[0];
        RtcFrame[4] = tBuf[1];
    }
    
    RtcFrame[5]		= '/';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.YEAR);
    
    if(tBuf[1] == 0)
    {
        RtcFrame[7] = tBuf[0];
        RtcFrame[6] = 0x30;
    }
    else
    {
        RtcFrame[6] = tBuf[0];
        RtcFrame[7] = tBuf[1];
    }

      RtcFrame[8] = '-';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.HOUR);

    if(tBuf[1] == 0)
    {
        RtcFrame[10] = tBuf[0];
        RtcFrame[9] = 0x30;
    }
    else
    {
        RtcFrame[9] = tBuf[0];
        RtcFrame[10] = tBuf[1];
    }

    RtcFrame[11] = ':';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.MIN);

    if(tBuf[1] == 0)
    {
        RtcFrame[13] = tBuf[0];
        RtcFrame[12] = 0x30;
    }
    else
    {
        RtcFrame[12] = tBuf[0];
        RtcFrame[13] = tBuf[1];
    }

    RtcFrame[14] = ':';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.SEC);
    
    if(tBuf[1] == 0)
    {
        RtcFrame[16] = tBuf[0];
        RtcFrame[15] = 0x30;
    }
    else
    {
        RtcFrame[15] = tBuf[0];
        RtcFrame[16] = tBuf[1];
    }

    RtcFrame[17] = '\0';

    strcat((char*)pcString, RtcFrame);
}

void GetTimeString(char* pcString)
{
    char tBuf[2];

    RdRTC(&AppStatus.DATE_TIME);

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.HOUR);

    if(tBuf[1] == 0)
    {
        TimeFrame[1] = tBuf[0];
        TimeFrame[0] = 0x30;
    }
    else
    {
        TimeFrame[0] = tBuf[0];
        TimeFrame[1] = tBuf[1];
    }

    TimeFrame[2] = ':';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.MIN);

    if(tBuf[1] == 0)
    {
        TimeFrame[4] = tBuf[0];
        TimeFrame[3] = 0x30;
    }
    else
    {
        TimeFrame[3] = tBuf[0];
        TimeFrame[4] = tBuf[1];
    }

    TimeFrame[5] = ':';

    sprintf(tBuf, "%d", AppStatus.DATE_TIME.SEC);

    if(tBuf[1] == 0)
    {
        TimeFrame[7] = tBuf[0];
        TimeFrame[6] = 0x30;
    }
    else
    {
        TimeFrame[6] = tBuf[0];
        TimeFrame[7] = tBuf[1];
    }

    TimeFrame[8] = '\0';

    strcat((char*)pcString, TimeFrame);
}

#ifdef RTC_ALM
void __ISR(_EXTERNAL_2_VECTOR, IPL3SOFT) _RTCINTHandler(void)
{
    SetALMTime();
//    IncTaskCount();
    INTClearFlag(INT_INT2);
}
#endif
#endif
