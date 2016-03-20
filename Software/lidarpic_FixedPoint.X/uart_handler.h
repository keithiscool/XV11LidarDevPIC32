/* 
 * File:   uart_handler.h
 * Author: Igor
 *
 * Created on July 5, 2015, 8:17 AM
 */

#ifndef UART_HANDLER_H
#define	UART_HANDLER_H


#define RING_BUF_SIZE 500
#define SIZE_OF_DMA_ARRAY 200


struct ringBuff
{
    unsigned char buf[RING_BUF_SIZE];
    int head;
    int tail;
    int count;
};

struct ringBuff buffer_five; //used for U5RX input UART (115200 baudrate from xv11 Lidar)
struct ringBuff buffer_one; //used for U1TX output to PC for serial debugging (after pic32 parsed the Lidar data)

bool stalled = true; 


void ring_buff_init(struct ringBuff* _this);
int ring_buff_size(struct ringBuff* _this);
unsigned char ring_buff_get(struct ringBuff* _this);
unsigned char ring_buff_peek(struct ringBuff* _this);
void ring_buff_put(struct ringBuff* _this, const unsigned char c);
void ring_buff_flush(struct ringBuff* _this, const int clearBuffer);
unsigned int modulo_inc(const unsigned int value, const unsigned int modulus);


//dma arrays for transmit (make a separate array for each DMA channel)
extern unsigned char dma_one_array[SIZE_OF_DMA_ARRAY];
extern void _queue_put(unsigned char *what, unsigned char how_many);

//
short DMA_Counter = 0;


#endif	/* UART_HANDLER_H */

