/* File:   queue.h
 * Author: Igor & Keith
 * Created on February 18, 2016, 4:26 PM
 */

#ifndef QUEUE_H
#define	QUEUE_H

#include <xc.h>
#include <string.h>
#include <stdbool.h>

struct dmaSettings
{
    unsigned char *dma_array;
    volatile unsigned int *dmacon;
    unsigned int con_busy_mask;
    unsigned int con_en_mask;
    volatile unsigned int *dmasize;
    volatile unsigned int *dmaecon;
    unsigned int econ_force_mask;
};


#define TOP_LEVEL_QUEUE_DEPTH 5
#define SECOND_LEVEL_QUEUE_DEPTH 193
#define SIZE_OF_DMA_ARRAY 200


struct Queue_handler {

    struct Queue_member {
        unsigned char buf[SECOND_LEVEL_QUEUE_DEPTH];
        int head;
        int tail;
        int count;
    };//queue[TOP_LEVEL_QUEUE_DEPTH];

    struct Queue_top {
        struct Queue_member queue[TOP_LEVEL_QUEUE_DEPTH];
        int head;
        int tail;
        int count;
    }send_queue;

//    struct Queue_top send_queue;
    unsigned int queueFullErrorCounter;
    unsigned char *dma_array;
    volatile unsigned int *dmacon;
    unsigned int con_busy_mask;
    unsigned int con_en_mask;
    volatile unsigned int *dmasize;
    volatile unsigned int *dmaecon;
    unsigned int econ_force_mask;
    int module_ID;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//Function definitions for DMA:
//(the underscore "_" in front of each function means that the function is meant to be used for DMA data transfers)

void _queue_init(void);
void _queue_data_put (unsigned char data);
unsigned char _queue_data_get(void);
unsigned int _modulo_inc(const unsigned int value, const unsigned int modulus);
//void _queue_begin(dmaSettings *_settings, int module_number);
void _queue_begin(struct dmaSettings *_settings, int module_number);
//void _queue_put(unsigned char * what, unsigned char how_many, unsigned char where, unsigned char from_where);
void _queue_put(unsigned char what, unsigned char how_many);
bool _queue_send(void);
unsigned int _fullError(void);


// object declaration for send struct
struct Queue_handler DMA_Buffer_One;

// declaration for dma arrays for transmit (make a separate array for each DMA channel)
unsigned char dma_one_array[SIZE_OF_DMA_ARRAY];

// declaration for dma settings
struct dmaSettings dmaOneSettings; // use only 1 DMA channel (increase array size to use more DMA channels)

// filled with class pointers to the send class
struct Queue_handler *queue_One_Pointer; // use only 1 DMA channel (increase array size to use more DMA channels)


#endif	/* QUEUE_H */