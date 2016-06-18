
//Modify softuart.h and remove the ATMEGA definitions
#ifndef FX2UART
#define FX2UART

#include "fx2regs.h"
#include "fx2types.h"





#define BUFFER_SIZE     4

// Init the Software Uart
void softuart_init(void);

// Clears the contents of the input buffer.
void softuart_flush_input_buffer( void );

// Tests whether an input character has been received.
unsigned char softuart_kbhit( void );

// Reads a character from the input buffer, waiting if necessary.
char softuart_getchar( void );

// To check if transmitter is busy
unsigned char softuart_transmit_busy( void );

// Writes a character to the serial port.
void softuart_putchar( const char );

// Turns on the receive function.
void softuart_turn_rx_on( void );

// Turns off the receive function.
void softuart_turn_rx_off( void );

// Write a NULL-terminated string from RAM to the serial port
void softuart_puts( const char *s );

// Write a NULL-terminated string from program-space (flash)
// to the serial port. example: softuart_puts_p(PSTR("test"))
void softuart_puts_p( const char *prg_s );

// Helper-Macro - "automatically" inserts PSTR
// when used: include avr/pgmspace.h before this include-file




void QueueInitTX(void);
__bit QueuePutTX(unsigned char data);
__bit QueueGetTX(unsigned char *old);
__bit QueueCheckTX();

void QueueInitRX(void);
__bit QueuePutRX(unsigned char data);
__bit QueueGetRX(unsigned char *old);
__bit QueueCheckRX();





#define softuart_puts_P(s___) softuart_puts_p(PSTR(s___))
#endif
