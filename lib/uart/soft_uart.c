/** \file lib/uart/soft_uart.c
 * This file is for defining a common API for accessing UARTs.
 * Remarks By Roaring(for the FX2LP from cypress)
 **/
#include <fx2regs.h>
#include <fx2macros.h>
#include <uart/api.h>
#include <assert.h>

/*Holds the value of the EA register*/
__bit ea_hold;
BOOL uartX_init(enum uart_baud rate, ...)
{
    //If clock is currently set to 48Mhz,return TRUE
    if(CPUFREQ!=CLK_48M)
	return FALSE;
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
