#ifndef FX2UART
#define FX2UART
#include "fx2regs.h"
#include "fx2types.h"
/**
 * putchar(char c) send the character out via PA2
 * This currently operates at about 115200
 * Higher baud rates could be achieved by decreasing the delay
 **/
void uart0_transmit(unsigned char c);
/**
 * uart0_init() sets the CPU clock frequency to 
 * 48Mhz. 
 **/
void uart0_init();
#endif

