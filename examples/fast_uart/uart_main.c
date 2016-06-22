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
#include <softuart.h>
#include <fx2ints.h>

unsigned long delay1;
void main(void)
{
    uart0_init();
    while (TRUE)
   {
      //uart_rx_service();
      uart_tx_service();
      //printf("H");
      uart0_transmit(0x24);
      for(delay1=0;delay1<2000;delay1++)
      {
          __asm
          nop
          __endasm;
      }
      uart_tx_service();
      for(delay1=0;delay1<2000;delay1++)
      {
          __asm
          nop
          __endasm;
      }
      uart_tx_service();
      for(delay1=0;delay1<2000;delay1++)
      {
          __asm
          nop
          __endasm;
      }
      uart_tx_service();
      for(delay1=0;delay1<2000;delay1++)
      {
          __asm
          nop
          __endasm;
      }

   }
}

void putchar(char c)
{
    uart0_transmit(c);
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
