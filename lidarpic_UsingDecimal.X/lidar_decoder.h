/* 
 * File:   lidar_decoder.h
 * Author: Igor
 *
 * Created on July 5, 2015, 8:14 AM
 */

#ifndef LIDAR_DECODER_H
#define	LIDAR_DECODER_H

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

bool transmission_in_progress = false;

float returned_speed = 0;
short SuccessfulMeasurements[360];
unsigned char InvalidFlag[4];
unsigned char WarningFlag[4];
short Distance[360];
short XCoordMeters[360];
short YCoordMeters[360];
unsigned short DegreeIndex;
short AnglesCoveredTotal;

#endif	/* LIDAR_DECODER_H */

