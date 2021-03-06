/* File:   lidar_decoder.h
 * Author: Igor
 * Created on July 5, 2015, 8:14 AM
 */

#ifndef LIDAR_DECODER_H
#define	LIDAR_DECODER_H

#include "interrupt_handler.h"
#include "SinLookupTable.h"

// To note for dsPIC32: 8bits=char   16bits=short    32bits=integer

struct ringBuff * data;
unsigned char buff_index = 0;
unsigned char data_buffer[22];
unsigned short *output_data;
unsigned short CRC_calculator(unsigned char * _this);
unsigned short assemble(unsigned char lower, unsigned char upper);

extern struct ringBuff buffer_five;

extern void ring_buff_init(struct ringBuff* _this);
extern int ring_buff_size(struct ringBuff* _this);
extern unsigned char ring_buff_get(struct ringBuff* _this);
extern unsigned char ring_buff_peek(struct ringBuff* _this);
extern void ring_buff_put(struct ringBuff* _this, const unsigned char c);
extern void ring_buff_flush(struct ringBuff* _this, const int clearBuffer);
extern unsigned int modulo_inc(const unsigned int value, const unsigned int modulus);
extern bool debugLidarPolarData(void);
extern bool debugLidarCartesianData(void);
extern int AllMeasurementsTaken(void);
extern bool LIDARdecode(short getDegrees[4]);

//Timer done global variables
extern bool timeFlagOneHundMilSec;
extern bool timeFlagFiveSec;

//Lidar Intra-packet (during packet parsing flag)
bool transmission_in_progress = false;

float returned_speed = 0;
unsigned char QualityFlag[4];
unsigned char InvalidFlag[4];
unsigned char WarningFlag[4];


short QualityArr[181];
short DistanceArr[181];
short PreviousDistanceArr[181];
short XCoordMilliMeters[181];
short YCoordMilliMeters[181];
short DegreeIndex = 0;
short AnglesCoveredTotal = 0;
bool LidarCalcPerm = false;
int BadReadings = 0;


////Fixed Point Math Local Variables
//unsigned int TrigValCosine = 0;
//unsigned int TrigValSine = 0;
//bool degreeNegativeCosFlag = false;
//bool degreeNegativeSinFlag = false;


////Make pointer to be used to check if the cartesian x,y point is positive or negative
//bool CosNegativeFlag = 0, SinNegativeFlag = 0;
//bool *negativeNumSinPTR = &SinNegativeFlag;
//bool *negativeNumCosPTR = &CosNegativeFlag;
////Use same lookup table for cosine to save space on pic32 microcontroller
//extern unsigned short GetMySinLookup16bit(short Degrees, bool *negativeNumSin);
//extern unsigned short GetMyCosLookup16bit(short Degrees, bool *negativeNumCos);


#endif	/* LIDAR_DECODER_H */

