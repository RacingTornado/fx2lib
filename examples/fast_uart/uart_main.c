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
#include <stdio.h>

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>
#include <fx2ints.h>
#include "mpsse.h"
#include "timer_lib.h"
#include "softuart.h"
#define SU_TRUE    1
#define SU_FALSE   0

// startbit and stopbit parsed internally (see ISR)
#define RX_NUM_OF_BITS (8)
#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL




volatile WORD bytes;
volatile __bit gotbuf;
volatile BYTE icount;
volatile __bit got_sud;
volatile unsigned char anotherone;
DWORD lcount;
__bit on;



extern void uart_config ();
extern void ProcessXmitData ();
extern void ProcessRecvData ();
extern void configure_timer ();
extern void start_timer ();
extern void timer_init ();
extern void set_tx_pin_high ();
extern void set_tx_pin_low ();
extern unsigned char get_rx_pin_status ();
extern void configure_drive (unsigned char a, unsigned char b);
extern void toggle_port_value (unsigned char a, unsigned char b);
extern void uart_rx_fill ();
extern void putchar_a (char a);
extern void i2c_addr_logic ();
extern char i2c_data_logic (unsigned char dummyportenselect);
extern void i2c_stop_logic ();
extern void spi_data_logic (unsigned char mosi_data_a,
                            unsigned char master_pin_a);
extern void spi_mosi_data_logic ();
extern void spi_miso_data_logic ();
extern void fast_uart(unsigned char a);
extern void temp_call ();
extern char xxy (char a, char b);
extern BOOL handle_mpsse ();
extern void timerlib_init(CLK_SPD clk);
extern void create_timer();
void call_me();
extern void service_timer();
extern void timer_start();
extern void uart_tx_service();
extern void uart_rx_service();
extern __xdata unsigned char interval;
extern __xdata unsigned short periodic;
extern unsigned char volatile tx_buffer;
extern unsigned char volatile rx_buffer;
extern unsigned char volatile tx_count;
extern unsigned char volatile rx_count;
extern unsigned char volatile tx_bits_sent;
extern unsigned char volatile rx_bits_rcvd;
extern unsigned char volatile tx_busy;
extern unsigned char volatile rx_busy;
extern void QueueInitTX(void);
extern __bit QueuePutTX(unsigned char data);
extern __bit QueueGetTX(unsigned char * old);
extern __bit QueueCheckTX();
extern void QueueInitRX(void);
extern __bit QueuePutRX(unsigned char data);
extern __bit QueueGetRX(unsigned char * old);
extern __bit QueueCheckRX();
extern __xdata unsigned char send_tx[BUFFER_SIZE];
extern __xdata unsigned char receive_rx[BUFFER_SIZE];
extern  __xdata  volatile unsigned short fx2_tick ;
extern  void (*callback)();






void
main ()
{
   //Setup data available and other init
   got_sud = FALSE;
   gotbuf = FALSE;
   bytes = 0;
   // renumerate
   RENUMERATE_UNCOND ();
   //Call our custom function to do our UART init
   uart_config ();
   sio0_init (57600);
   //Enable USB auto vectored interrupts
   USE_USB_INTS ();
   ENABLE_SUDAV ();
   ENABLE_SOF ();
   ENABLE_HISPEED ();
   ENABLE_USBRESET ();
   EA = 1;			// global interrupt enable
   //5us interval
   interval = 5;
   timerlib_init(CLK_48M);
   timer_start();
   periodic = 20;
   callback = call_me;
   create_timer();
   softuart_init ();
   start_timer ();
   ENABLE_TIMER1 ();
   TR0 = 1 ;

   while (TRUE)
   {
      service_timer();
      uart_rx_service();
      uart_tx_service();
      fast_uart(0x30);

      if (anotherone > 0 )
      {
         handle_setupdata ();
         anotherone --;
      }
   }
}

// copied routines from setupdat.h



BOOL
handle_get_descriptor ()
{
   return FALSE;
}



// value (low byte) = ep
#define VC_EPSTAT 0xB1



BOOL
handle_vendorcommand (BYTE cmd)
{
   switch (cmd)
   {
      case VC_EPSTAT:
      {
         __xdata BYTE * pep = ep_addr (SETUPDAT[2]);
         printf ("ep %02x\n", *pep);

         if (pep)
         {
            EP0BUF[0] = *pep;
            EP0BCH = 0;
            EP0BCL = 1;
            return TRUE;
         }
      }
      break;

      default:
         handle_mpsse ();
         printf ("Need to implement vendor command: %02x\n", cmd);
   }

   return FALSE;
}



// this firmware only supports 0,0
BOOL
handle_get_interface (BYTE ifc, BYTE * alt_ifc)
{
   printf ("Get Interface\n");

   if (ifc == 0)
   {
      *alt_ifc = 0;
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}



BOOL
handle_set_interface (BYTE ifc, BYTE alt_ifc)
{
   printf ("Set interface %d to alt: %d\n", ifc, alt_ifc);

   if (ifc == 0 && alt_ifc == 0)
   {
      // SEE TRM 2.3.7
      // reset toggles
      RESETTOGGLE (0x02);
      RESETTOGGLE (0x86);
      // restore endpoints to default condition
      RESETFIFO (0x02);
      EP2BCL = 0x80;
      SYNCDELAY;
      EP2BCL = 0X80;
      SYNCDELAY;
      RESETFIFO (0x86);
      return TRUE;
   }
   else
      return FALSE;
}



__bit on5;
__xdata WORD sofct = 0;
void
sof_isr ()
__interrupt SOF_ISR __using 1
{
   ++sofct;

   if (sofct == 8000)
   {
      // about 8000 sof interrupts per second at high speed
      on5 = !on5;

      if (on5)
      {
         d5on ();
      }
      else
      {
         d5off ();
      }

      sofct = 0;
   }

   CLEAR_SOF ();
}



// get/set configuration
BYTE
handle_get_configuration ()
{
   return 1;
}



BOOL
handle_set_configuration (BYTE cfg)
{
   return cfg == 1 ? TRUE : FALSE;	// we only handle cfg 1
}



// copied usb jt routines from usbjt.h
void
sudav_isr ()
__interrupt SUDAV_ISR
{


   got_sud = TRUE;
   anotherone++;
   CLEAR_SUDAV ();
}



void usbreset_isr ()
__interrupt USBRESET_ISR
{
   handle_hispeed (FALSE);
   CLEAR_USBRESET ();
}



void hispeed_isr ()
__interrupt HISPEED_ISR
{
   handle_hispeed (TRUE);
   CLEAR_HISPEED ();
}



void timer1_isr ()
__interrupt TF1_ISR
{


}



void timer0_isr () __interrupt TF0_ISR
{

   fx2_tick++;


}



void call_me()
{
   __asm
   mov _OEA, #0x08
   cpl _PA3
   __endasm;
}


