/* File:   uart_handler.h
 * Author: Igor
 * Created on July 5, 2015, 8:17 AM
 */

#ifndef UART_HANDLER_H
#define	UART_HANDLER_H



//Ring Buffer parameters for input UART (UART 4: U4RX)
#define UART_BUFFER_SIZE 200



void *memset(void *s, int c, size_t n);

struct UART_ring_buff {
    unsigned char buf[UART_BUFFER_SIZE];
    int head;
    int tail;
    int count;
};




void UART_init(void);
void UART_buff_init(struct UART_ring_buff* _this);
void UART_buff_put(struct UART_ring_buff* _this, const unsigned char c);
unsigned char UART_buff_get(struct UART_ring_buff* _this);
void UART_buff_flush(struct UART_ring_buff* _this, const int clearBuffer);
int UART_buff_size(struct UART_ring_buff* _this);
unsigned int UART_buff_modulo_inc(const unsigned int value, const unsigned int modulus);
unsigned char UART_buff_peek(struct UART_ring_buff* _this);


unsigned char Receive_peek(void);
int Receive_available(void);
unsigned char Receive_get(void);
void Send_put(unsigned char _data);


#endif	/* UART_HANDLER_H */