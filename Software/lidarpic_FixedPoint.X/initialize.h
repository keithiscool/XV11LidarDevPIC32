/* File:   initialize.h
 * Author: Igor
 * Created on July 3, 2015, 1:14 PM
 */

#ifndef INITIALIZE_H
#define	INITIALIZE_H


#include "queue.h"


#define RING_BUF_SIZE 200


// prototype for input ring buffer
struct ringBuff
{
    unsigned char buf[RING_BUF_SIZE];
    int head;
    int tail;
    int count;
};

extern struct ringBuff buffer_five;
extern int returned_data[10];

void initialize(void);
void timers(void);
void PWM(void);
void UART(void);
void DMA(void);

extern void beginLIDARdecoder(unsigned int * _output_data, struct ringBuff* _input_data);

extern void delay(void);


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DMA Variables and Definitions Below:

//////// prototype for setting to pass to dma class
//////struct dmaSettings
//////{
//////    unsigned char *dma_array;
//////    volatile unsigned int *dmacon;
//////    unsigned int con_busy_mask;
//////    unsigned int con_en_mask;
//////    volatile unsigned int *dmasize;
//////    volatile unsigned int *dmaecon;
//////    unsigned int econ_force_mask;
//////};


//dma arrays for transmit (make a separate array for each DMA channel)
extern unsigned char dma_one_array[SIZE_OF_DMA_ARRAY];

// filled with class pointers to the send class
extern struct Queue_handler *queue_One_Pointer; // use only 1 DMA channel (increase array size to use more DMA channels)

// object declaration for send struct
extern struct Queue_handler DMA_Buffer_One;

// declaration for dma settings
extern struct dmaSettings dmaOneSettings; // use only 1 DMA channel (increase array size to use more DMA channels)

//Use the ring buffer for outputting to the DMA (DMA channel 1)
extern struct ringBuff buffer_one;


#endif	/* INITIALIZE_H */

