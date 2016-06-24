/** \file include/uart/soft_uart.h
 * This file contains a MACRO for defining UART function wrappers
 **/

#ifndef SOFT_UART_H
#define SOFT_UART_H



#define CREATE_FAST_UART(uart0,pinname)			\
__sbit __at  pinname TX_PIN;				\
BOOL uart0##_init(enum uart_baud rate,...)		\
{__asm							\
.equ _TX_PIN , _PA2					\
__endasm;						\
return TRUE;						\
}							\
void uart0##_tx(char c) {				\
uart_tx(c);						\
}

/**
 * \brief Send data out via UART
**/
void uart_tx(char c);

#endif

