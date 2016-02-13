/* 
 * File:   initialize.h
 * Author: Igor
 *
 * Created on July 3, 2015, 1:14 PM
 */

#ifndef INITIALIZE_H
#define	INITIALIZE_H

//#define RING_BUF_SIZE 100
//
//extern struct ringBuff
//{
//    unsigned char buf[RING_BUF_SIZE];
//    int head;
//    int tail;
//    int count;
//};

extern struct ringBuff buffer_five;
extern int returned_data[10];

void initialize(void);
void timers(void);
void PWM(void);
void UART(void);

extern void beginLIDARdecoder(unsigned int * _output_data, struct ringBuff* _input_data);

extern void delay(void);

#endif	/* INITIALIZE_H */

