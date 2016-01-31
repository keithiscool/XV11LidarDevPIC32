/* 
 * File:   uart_handler.h
 * Author: Igor
 *
 * Created on July 5, 2015, 8:17 AM
 */

#ifndef UART_HANDLER_H
#define	UART_HANDLER_H


#define RING_BUF_SIZE 500

struct ringBuff
{
    unsigned char buf[RING_BUF_SIZE];
    int head;
    int tail;
    int count;
};

struct ringBuff buffer_five;
struct ringBuff buffer_one; 

bool stalled = true; 


void ring_buff_init(struct ringBuff* _this);
int ring_buff_size(struct ringBuff* _this);
unsigned char ring_buff_get(struct ringBuff* _this);
unsigned char ring_buff_peek(struct ringBuff* _this);
void ring_buff_put(struct ringBuff* _this, const unsigned char c);
void ring_buff_flush(struct ringBuff* _this, const int clearBuffer);
unsigned int modulo_inc(const unsigned int value, const unsigned int modulus);


bool Done_Printing_Debug(void);


#endif	/* UART_HANDLER_H */

