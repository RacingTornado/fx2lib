/** \file include/uart/soft_uart.h
 * This file contains a MACRO for defining UART function wrappers
 **/

#ifndef SOFT_UART_H
#define SOFT_UART_H

#define load_delay unsigned char 
/**
 * \brief Automatically generates the function calls to allow multiple
 * UARTS to be created. The main parameters for the UART is the pin number
 * as well as the speed of the operation. These should be passed as an argument
 * to the macro. The load_delay element controls the speed. This is created with the
 * uart name . 
 **/
#define CREATE_FAST_UART(uart0,pinname,bitnum)		\
unsigned char uart0##_load_delay;			\
BOOL uart0##_init(enum uart_baud rate,...)		\
{							\
uartX_init(rate);					\
return TRUE;						\
}							\
void uart0##_tx(char c) {				\
__asm							\
.equ _TX_PIN1,pinname					\
.equ _load_delay,_##uart0##_load_delay			\
__endasm;						\
OEA = bitnum;						\
uart_tx(c);						\
}							\
BOOL uart0##_set_baud(enum uart_baud rate) {		\
    switch(rate)					\
    {							\
        case BAUD_2400:					\
            break;					\
        case BAUD_4800:					\
            break;					\
        case BAUD_9600:					\
            break;					\
        case BAUD_19200:				\
            uart0##_load_delay = 0xd0;			\
            break;					\
        case BAUD_38400:				\
            uart0##_load_delay = 0x68;			\
            break;					\
        case BAUD_57600:				\
            uart0##_load_delay = 0x45;			\
            break;					\
        case BAUD_115200:				\
            uart0##_load_delay = 0x20;			\
            break;					\
        case BAUD_ANY:					\
            break;					\
        case BAUD_FASTEST:				\
            break;					\
        default:					\
            uart0##_load_delay = 0x20;			\
            break;					\
    }							\
    return TRUE;					\
}							\
BOOL uart0##_init(enum uart_baud rate,...);		\
void uart0##_tx(char c);				\
BOOL uart0##_set_baud(enum uart_baud rate);

static inline void uart_tx(char c)
{
    //Done in ASM to improve performance. It takes only 6
    //cycles to move the data out, however a delay has been
    //introduced in order to get a baud rate of 115200
    //The mask which is to be written into the pin
    //An efficient UART bitbang routine in assembly
    __asm
    //Like #define in C. Can easily be used to change the pin
    //.equ _TX_PIN, _PA2
    //Disable interrupts
    //This is used because timing is critical
    //If the FX2 jumps into the ISR temporarily , it may cause transmit
    //errors. By clearing EA, we can disable interrupts
    jnb _EA,0001$//Check if the EA bit is set to 1 or not 
    setb _ea_hold        //Store the EA bit
    sjmp 0002$
    0001$:
    clr _ea_hold        //Store the EA bit
    0002$:              
    clr _EA             //(2 cycles)
    //Move the data to be sent into the ACC
    //The data which is to be shifted out is held in the dpl register
    //We move the data into A for easy access to subsequent instructions
    mov a , dpl         //(2 cyles)
    clr c               //(1 cycle )
    //We need to send out 8 bits of data
    //Load r0 with value 8
    mov r0, #0x08       //(2 cycles)
    //Create the start bit
    clr _TX_PIN1         //(2 cycles)
    //Precalculated delay since 1 cycle takes 88ns
    //At 12Mhz, it should be about 83.33ns
    //But it appears to be about 88ns
    //These numbers have been verified using an analyzer
    mov r1, _load_delay     //(2 cycles)
    0003$:
    //1 bit is about 8.6us
    djnz r1, 0003$      //(3 cycles)
    //DJNZ on Rn takes 3 cycles
    //NOP takes about 1 cycle
    //Add 2 more cycles of delay
    //97 cycles
    nop                 //(1 cycle )
    nop                 //(1 cycle )
    0004$:
    rrc a               //(2 cycles)
    //The above rotates the accumulator right through the carry
    //Move the carry into the port
    mov _TX_PIN1, c      //(2 cycles)
    //Now we need to add delay for the next
    mov r1, _load_delay //(2 cycles)
    //31*3 , 93 cycles of delay
    0005$:
    djnz r1, 0005$      //(3 cycles)
    nop                    //(1 cycle )
    //3 more cycles of delay
    //97 cycles
    djnz r0, 0004$  //(3 cycles)
    setb _TX_PIN1        //(2 cycles) 
    //This is for stop bit
    //We need to delay the stop bit,  otherwise we may get errors.
    mov r1, _load_delay     //(2 cycles)
    0006$:
    djnz r1, 0006$      //(3 cycles) 
    //for DJNZ , Jump for 32*3 , 96 cycles
    nop                 //(1 cycle )       
    //97 cycles of delay
    mov c,_ea_hold      //Restore the EA register to enable or disable interrupts
    mov _EA,c           //(2 cycles)
    __endasm;
}

#endif
