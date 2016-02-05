/*
 * File:   pUART.h
 * Author: Srivardhan
 *
 * Created on 14 September, 2013, 10:58 AM
 */

#include<plib.h>

#ifndef PUART_H
#define	PUART_H

#ifdef	__cplusplus
extern "C" {
#endif


/**
 * \brief UART (data) rate
 */
typedef enum {
    COM_UART_RATE_1K2 = 1200,           /**< UART data rate 1'200 bps (HART) */
    COM_UART_RATE_9K6 = 9600,           /**< UART data rate 9'600 bps (default) */
    COM_UART_RATE_19K = 19200,          /**< UART data rate 19'200 bps */
    COM_UART_RATE_38K = 38400,          /**< UART data rate 38'400 bps */
    COM_UART_RATE_57K = 57600,          /**< UART data rate 57'600 bps */
    COM_UART_RATE_115K = 115200,        /**< UART data rate 115'200 bps */
} COM_UART_RATE;

#define UART2A_TX_IRQ   _UART2A_TX_IRQ
#define UART3_TX_IRQ    _UART3_TX_IRQ

    /**
 * \brief UART module
 */
typedef enum
{
    //COM_UART_MODULE_1 = UART1,          /**< UART module #1 */
    COM_UART_MODULE_2 = UART2,          /**< UART module #2 (default) */
    COM_UART_MODULE_3 = UART3,          /**< UART module #3 (default) */
    COM_UART_MODULE_4 = UART4,
    COM_UART_MODULE_6 = UART6,
} COM_UART_MODULE;

typedef enum {
    COM_UART_MODE_8N1 = UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1,    /**< UART line mode 8N1 (default) */
    COM_UART_MODE_8O1 = UART_DATA_SIZE_8_BITS | UART_PARITY_ODD | UART_STOP_BITS_1,     /**< UART line mode 8O1 */
    COM_UART_MODE_8E1 = UART_DATA_SIZE_8_BITS | UART_PARITY_EVEN | UART_STOP_BITS_1,    /**< UART line mode 8E1 */
} COM_UART_MODE;

extern void ConfigUART(void);
extern void PutStrUART(char *pcString);

#ifdef	__cplusplus
}
#endif

#endif	/* PUART_H */

