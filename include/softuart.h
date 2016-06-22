#ifndef FX2UART
#define FX2UART
#include "fx2regs.h"
#include "fx2types.h"

/**
 * uart0_init() sets the CPU clock frequency to
 * 48Mhz.
 **/
void uart0_init();
void timer_init();
void softuart_init( void );
void uart_tx_service();
void uart_rx_service();
void QueueInitTX(void);
__bit QueuePutTX(unsigned char data);
__bit QueueGetTX(unsigned char  *old);
__bit QueueCheckTX();
void QueueInitRX(void);
__bit QueuePutRX(unsigned char data);
__bit QueueGetRX(unsigned char * old);
__bit QueueCheckRX();
void uart0_transmit(char c);
void process_isr();
extern unsigned char volatile tx_buffer;
extern unsigned char volatile rx_buffer;
extern unsigned char volatile tx_count;
extern unsigned char volatile rx_count;
extern unsigned char volatile tx_bits_sent;
extern unsigned char volatile rx_bits_rcvd;
extern unsigned char volatile tx_busy;
extern unsigned char volatile rx_busy;
#endif

