/** \file include/uart/api.h
 * This file is for defining a common API for accessing UARTs.
 **/

#ifndef UART_API_H
#define UART_API_H

#include "fx2types.h"
#include "stdarg.h"

/**
 * \brief Initalizes UART.
 * Returns TRUE if initialization is successful.
 * \param Rate See uartX_set_baud().
 **/
BOOL uartX_init(enum uart_baud rate, ...);

/**
 * \brief enum Standard available baud rates.
 * BAUD_FASTEST will give you the fastest baud rate the UART can operate at. WARNING:
 * This is allowed to be a non-standard baud rate.
 * Enum values greater than 0 are supported depending on the UART in use.To check
 * if the baud is supported call uartX_set_baud and use the return value to determine
 * if the mode is supported.
**/
typedef enum
{
	BAUD_FASTEST = -2,	///< Fastest baud rate the UART can operate at. WARNING:This is allowed to be a non-standard baud rate.
	BAUD_ANY = -1,		///< Virtual baud, Most efficient BAUD rate for the UART.
	BAUD_INVALID = 0,	///< A divider between actual baud rates and virtual baud rates.
	BAUD_2400 = 1,		///< Sets the baud rate to 2400.
	BAUD_4800 = 2,		///< Sets the baud rate to 4800.
	BAUD_9600 = 3,		///< Sets the baud rate to 9600.
	BAUD_19200 = 4,		///< Sets the baud rate to 19200.
	BAUD_38400 = 5,		///< Sets the baud rate to 38400.
	BAUD_57600 = 6,		///< Sets the baud rate to 57600.
	BAUD_115200 = 7		///< Sets the baud rate to 115200.
} uart_baud;

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
 * Returns TRUE if successful.
**/
BOOL uartX_set_baud(uart_baud rate);

/**
 * \brief Returns the baud rate currently being used.
**/
uart_baud uartX_get_baud();

/**
 * \brief Transmits data through UART.
 * \param c The character to be sent out.
**/

void uartX_tx(char c);

/**
 * \brief Returns if the next transmit call will block.
 * FALSE - Non Blocking.
 * TRUE - Blocking.
**/

BOOL uartX_tx_will_block();

/**
 * \brief Returns how many more bytes can be loaded into the transmit buffer.
**/
BYTE uartX_tx_queue_len();

/**
 * \brief Receives data through UART.
 * Returns one byte at a time from the receive queue.
**/
char uartX_rx();

/**
 * \brief Returns if the next receive call will block.
 * FALSE - Non Blocking.
 * TRUE - Blocking.
**/
BOOL uartX_rx_will_block();

/**
 * \brief Returns count number of bytes present in the receive buffer.
**/
BYTE uartX_rx_queue_len();

#endif
