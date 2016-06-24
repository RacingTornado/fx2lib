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

//We need this declaration
void uart0_tx(char c);

//Initialize UART, call it uart0 and set the tx pin on PA1
CREATE_FAST_UART(uart0,0x82)

//Used for setting the baud rate.
enum uart_baud baud;
void main(void)
{
    baud = BAUD_115200;
    uartX_init(baud);
    uartX_set_baud(baud);
    SETCPUFREQ(CLK_48M);
    while (TRUE)
    {
        printf("Hello\r\n");
    }
}

void putchar(char c)
{
    uart0_tx(c);
}
