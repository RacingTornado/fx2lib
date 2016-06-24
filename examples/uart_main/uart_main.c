/**
 * Copyright (C) 2009 Ubixum, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 **/
#include <fx2regs.h>
#include <delay.h>
#include <stdio.h>
#include <uart/api.h>
#include <uart/soft_uart.h>
#include <fx2macros.h>
#include <fx2types.h>
#include <assert.h>

//Initialize UART, call it uart0 and set the tx pin on PA1.
CREATE_FAST_UART(uart0,OEB,_PB0,bmBIT0,TRUE,FALSE)
CREATE_FAST_UART(uart1,OEA,_PA4,bmBIT4,TRUE,FALSE)
//Used for setting the baud rate.
enum uart_baud baud;

void main(void)
{
    baud = BAUD_115200;
    SETCPUFREQ(CLK_48M);
    while (!uart0_init(baud));
    while (!uart1_init(baud));
    uart0_set_baud(baud);
    baud = BAUD_115200;
    uart1_set_baud(baud);
    while (TRUE)
    {
        printf("Hello\r\n");
        uart1_tx(0x44);
    }
}

void putchar(char c)
{
    uart0_tx(c);
}
