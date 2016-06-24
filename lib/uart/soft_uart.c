/** \file lib/uart/soft_uart.c
 * This file is for defining a common API for accessing UARTs.
 * Remarks By Roaring(for the FX2LP from cypress)
 **/
#include <fx2regs.h>
#include <fx2macros.h>
#include <uart/api.h>
#include <assert.h>

/*This is set by calling the uartX_set_baud() function.*/
unsigned char load_delay;
/*Holds the value of the EA register*/
__bit ea_hold;
BOOL uartX_init(enum uart_baud rate, ...)
{
    unsigned char clk_cmp;
    //Clear all bits except the CLKSPD[1:0]
    clk_cmp = CPUCS & 0x18;
    //If clock is currently set to 48Mhz,return TRUE
    if(clk_cmp == 0x10)
    	return TRUE;
    return FALSE;
}


void uart_tx(char c)
{
    //Done in ASM to improve performance. It takes only 6
    //cycles to move the data out, however a delay has been
    //introduced in order to get a baud rate of 115200
    //The mask which is to be written into the pin
    OEA |= 0x04;
    //An efficient UART bitbang routine in assembly
    __asm
    //Like #define in C. Can easily be used to change the pin
    //.equ _TX_PIN, _PA2
    //Disable interrupts
    //This is used because timing is critical
    //If the FX2 jumps into the ISR temporarily , it may cause transmit
    //errors. By clearing EA, we can disable interrupts
    jnb _EA,set_ea_enable//Check if the EA bit is set to 1 or not 
    setb _ea_hold        //Store the EA bit
    sjmp start
    set_ea_enable:
    clr _ea_hold        //Store the EA bit
    start:              
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
    clr _TX_PIN         //(2 cycles)
    //Precalculated delay since 1 cycle takes 88ns
    //At 12Mhz, it should be about 83.33ns
    //But it appears to be about 88ns
    //These numbers have been verified using an analyzer
    mov r1, _load_delay     //(2 cycles)
    delay_1$:
    //1 bit is about 8.6us
    djnz r1, delay_1$      //(3 cycles)
    //DJNZ on Rn takes 3 cycles
    //NOP takes about 1 cycle
    //Add 2 more cycles of delay
    //97 cycles
    nop                 //(1 cycle )
    nop                 //(1 cycle )
    next_bit$:
    rrc a               //(2 cycles)
    //The above rotates the accumulator right through the carry
    //Move the carry into the port
    mov _TX_PIN, c      //(2 cycles)
    //Now we need to add delay for the next
    mov r1, _load_delay //(2 cycles)
    //31*3 , 93 cycles of delay
    delay_2$:
    djnz r1, delay_2$      //(3 cycles)
    nop                    //(1 cycle )
    //3 more cycles of delay
    //97 cycles
    djnz r0, next_bit$  //(3 cycles)
    setb _TX_PIN        //(2 cycles) 
    //This is for stop bit
    //We need to delay the stop bit,  otherwise we may get errors.
    mov r1, _load_delay     //(2 cycles)
    delay_3$:
    djnz r1, delay_3$      //(3 cycles) 
    //for DJNZ , Jump for 32*3 , 96 cycles
    nop                 //(1 cycle )       
    //97 cycles of delay
    mov c,_ea_hold      //Restore the EA register to enable or disable interrupts
    mov _EA,c           //(2 cycles)
    __endasm;
}

BOOL uartX_set_baud(enum uart_baud rate)
{
    switch(rate)
    {
        case BAUD_2400:
            load_delay = 0xd0;
            break;
        case BAUD_4800:
            break;
        case BAUD_9600:
            break;
        case BAUD_19200:
            load_delay = 0xd0;
            break;
        case BAUD_38400:
            load_delay = 0x68;
            break;
        case BAUD_57600:
            load_delay = 0x45;
            break;
        case BAUD_115200:
            load_delay = 0x20;
            break;
        case BAUD_ANY:
            break;
        case BAUD_FASTEST:
            break;
        default:
            load_delay = 0x20;
            break;
    }
    return TRUE;
}

enum uart_baud uartX_get_baud()
{
    return BAUD_115200;
}

BOOL uartX_tx_willblock()
{
	return TRUE;
}

char uartX_rx()
{
 //This function should never be called
 assert(FALSE);
 return 0xFF;
}

BOOL uartX_check_rx_blocking()
{
 return TRUE;
}

BYTE uartX_check_receive_buffer()
{
 //Read not implemented. No data is present in the buffer
 return 0x00;
}
