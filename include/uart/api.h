/** \file include/uart/api.h
 * This file is for defining a common API for accessing UARTs.
 **/

#ifndef UART_API_H
#define UART_API_H

#include "fx2types.h"
#include "stdarg.h"

/**
 * enum Standard available baud rates
 *
**/
enum uart_baud { BAUD_2400, BAUD_4800, BAUD_9600, BAUD_19200, BAUD_38400, BAUD_57600, BAUD_115200, BAUD_ANY, BAUD_FASTEST };

/**
 * \brief initalizes UART.
 * Returns 0 if initialization is successful.
 * \param rate See uartX_set_baud()
 **/
BOOL uartX_init(enum uart_baud rate, ...);

/**
 * \brief Sets the UART baud rate to one of the allowed parameters.
 *   Possible Baud rates:
 *    \li 2400
 *    \li 4800
 *    \li 9600
 *    \li 19200
 *    \li 28800
 *    \li 38400
 *    \li 57600
 *    \li 115200
 * Returns 0 if successful. 
**/
BOOL uartX_set_baud(enum uart_baud rate);

/**
 * \brief Returns the baud rate currently being used.
**/
enum uart_baud uartX_get_baud();

/**
 * \brief transmits data through UART 
 * \param c The character to be sent out
**/

void uartX_tx(char c);

/**
 * \brief Returns if the transmit is blocking or not
 * TRUE - Blocking
 * FALSE -Non Blocking
**/

BOOL uartX_tx_willblock();

/**
 * \brief receives data through UART.
 * Returns one byte at a time from the queue
 *
**/
char uartX_rx();

/**
 * \brief Returns if the receive is blocking or not
 * 0 - Non Blocking
 * 1 - Blocking
**/
BOOL uartX_check_rx_blocking();

/**
 * \brief Returns count number of bytes present in the buffer
 *
**/
BYTE uartX_check_receive_buffer();

#endif
