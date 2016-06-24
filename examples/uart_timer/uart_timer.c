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
#include <stdio.h>
#include <fx2ints.h>
#include <uart/api.h>
#include <uart/timer_uart.h>
#include <fx2macros.h>

//Used for setting the baud rate.
//Currently unimplemented
enum uart_baud baud;
//Extern declaration. 
extern void process_isr();
void main(void)
{
    baud = BAUD_9600;
    uartX_init(baud);
    printf("Hello");
    while (TRUE)
   {
      uart_rx_service();
      uart_tx_service();

   }
}

void putchar(char c)
{
    uartX_tx(c);
}


/**
 * \brief This function is actually an ISR
 * It is called periodically to check if data is ready to be transmitted
 * The receive logic looks at the rx pin, sampling it continously. The moment
 * a start bit is detected, it begins shifting the data in
 * and finally sets a flag stating the receive is complete. This flag is reset
 * in the uart_rx_service. This helps achieve non blocking behaviour.
 **/
void timer1_isr ()
__interrupt TF1_ISR
{
 process_isr();
}
