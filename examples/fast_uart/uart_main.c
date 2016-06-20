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
#include <i2c_utils.h>
#define SU_TRUE    1
#define SU_FALSE   0
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
unsigned char fx2_tick;
extern void fast_uart(unsigned char a, unsigned char b);



void i2c_control();
void i2c_init();
void configure_start_timer();
/**************************************************
I2C declarations
***************************************************/
enum isr_state
        {
        state_tx=0, //0
        state_rx=1 ,  //1
        state_wait=2
        };
enum isr_state tx_rx;
unsigned char tx_i2c_buffer;
unsigned char rx_i2c_buffer;
unsigned char bit_count;
enum i2c_states
        {
idle = 0,
start = 1,
address = 2,
addr_ack = 3,
data_write = 4,
data_write_ack = 5,
data_read = 6,
data_read_ack = 7,
stop = 8,
read_addr_ack = 9,
read_data_ack = 10,
        };
//These variables hold data for the current
//transaction. Once the transaction is complete
//they are inserted back into the queue or the
//queue is cleared , depending on whether it is
//a read or write operation.
unsigned char retries;
unsigned char addr[I2C_ADDR];
unsigned char data[I2C_DATA];
unsigned char addr_length;
unsigned char data_length;
//I2C state variable
enum i2c_state my_i2c_states;
//Temporary counters for EEPROM access
unsigned char i ;
unsigned char j;
//Loaded automatically from the
//address value(first byte)
__bit rw_bit;
unsigned char read_write;
unsigned long delay1;
//Buffer to load up the data and insert into
//the i2c_client queue
__xdata unsigned char write_addr[I2C_ADDR];
__xdata unsigned char write_data[I2C_DATA];


void main() {

  //Setup data available and other init
   got_sud = FALSE;
   gotbuf = FALSE;
   bytes = 0;
   // renumerate
   RENUMERATE_UNCOND ();
   sio0_init (57600);
   CPUCS = 0x10;
   //Enable USB auto vectored interrupts
   USE_USB_INTS ();
   ENABLE_SUDAV ();
   ENABLE_SOF ();
   ENABLE_HISPEED ();
   ENABLE_USBRESET ();
   EA = 1;			// global interrupt enable
   /********************************
   I2C BLOCK BITBANG
   *********************************/
   i2c_init();
   configure_start_timer();
   ENABLE_TIMER1();




   while (TRUE)
    {
        write_addr[0] = 0x20;
        write_addr[1] = 0x00;
        write_addr[2] = 0x00;
        write_data[0] = 0xab;
        i2c_control();
        if (anotherone > 0 )
        {
            handle_setupdata ();
            anotherone --;
        }

    }

}



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
         //handle_mpsse ();
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
    __asm
    mov a,_tx_rx
    CJNE A, #0x02, state //If in halt state, do nothing
    ajmp finish
    state:
    djnz _bit_count,cont;
    mov _tx_rx,#0x02
    ajmp finish
    cont:
    orl _OEA,#0x40
    clr _PA6
    mov a,_tx_rx
    CJNE A, #0x00, rx
    tx:
    orl _OEA,#0x80
    mov a, _tx_i2c_buffer;
    rlc a;
    mov _PA7, c;
    mov _tx_i2c_buffer,a;
    sjmp sclh//Jump back
    rx:
    anl _OEA,#0x7f
    mov a, _tx_i2c_buffer;
    mov c,_PA7;
    rlc a;
    mov _tx_i2c_buffer,a;
    nop;
    sclh:
    setb _PA6
    finish:
    nop
    __endasm;


}



void timer0_isr () __interrupt TF0_ISR
{

   fx2_tick++;


}



void configure_start_timer()
{


   TMOD = 0x20;
   SYNCDELAY;
   TR1 = 0;
   SYNCDELAY;
   TH1 = 0xc3;
   SYNCDELAY;
   TL1 = 0x23;
   SYNCDELAY;
   TR1 = 1;
   SYNCDELAY;

}











