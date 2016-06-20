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

// New variables my_i2c_states
//i2c_start
//tx_i2c_bits
//data_rw_bit
// tx_i2c_buffer_load
//tx_i2c_buffer
//ack_bit
//data_pending_bit
//i2c_stop maybe also needed.
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
extern unsigned char retries;
extern unsigned char addr;
extern unsigned char data[5];
extern unsigned char length;
unsigned char  load_data[5];
unsigned char load_addr;
enum i2c_state my_i2c_states;
unsigned char i ;
unsigned char j;
__bit rw_bit;
/***********************
I2C Function
************************/
extern void I2CInit(void);
extern __bit I2CPutTX(unsigned char addr, unsigned char * data);
extern __bit I2CGetTX();
extern __bit I2CCheckTX();




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


   configure_start_timer();
   ENABLE_TIMER1();
   i2c_init();
   while (TRUE)
   {
      //fast_uart(0x14,0x04);
//      load_data[0]=0x44;
//      load_data[1]=0x45;
//      load_data[2]=0x23;
//      load_data[3]=0x87;
//      load_data[4]=0x65;
//      load_addr = 0x34;
//
//      I2CPutTX(load_addr,&load_data[0]);
//      I2CGetTX();
//      fast_uart(addr,0x04);
//      fast_uart(data[2],0x04);
      addr = 0x20;
      data[0]= 0x76;
      data[1]= 0x43;
      length = 1;
      retries = 3;
      //i2c_service();
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
    setb _PA7
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



void i2c_service()
{
    if(tx_rx == state_wait)
    {
        //fast_uart(0x20);
        tx_i2c_buffer = 0xA1;
        bit_count = 0x09;
        tx_rx = state_tx;
    }
    else
    {
        //fast_uart(0x30);
        //tx_rx = state_tx;
    }
}



void i2c_init()
{
    my_i2c_states = idle;
    I2CInit();
    i = 0;
    OEA |= 0xc0;
    PA6 = 1;
    PA7 = 1;
}



void i2c_control()
{
    if ((my_i2c_states == idle) && (tx_rx == state_wait))
    {
        // Look for non empty queue and pull the SDA low keeping SCL high.
        if (I2CCheckTX() == 0)
        {
            my_i2c_states = start;
        }
        else
        {
        }
    }
    else if ((my_i2c_states == start) && (tx_rx == state_wait))
    {
        //Send the start bit
        OEA |= 0xC0;
        PA7 = 0 ;
        //Initial delay between the SDA and SCL lines for start bit
        __asm
        mov r0, #0x03
        001$:djnz r0,001$
        __endasm;
        PA6 = 0;
        my_i2c_states = address;
        //fast_uart(0x55,0x04);

        //I2CGetTX();
    }
    else if ((my_i2c_states == address) && (tx_rx == state_wait))
    {
        //Send the address and check the R/W bit
        tx_i2c_buffer = addr;
        bit_count = 0x09;
        tx_rx = state_tx;
        __asm
        mov a, _addr
        rrc a
        mov _rw_bit,c
        __endasm;
        my_i2c_states = read_addr_ack;
        //fast_uart(0x66,0x04);
    }
    else if ((my_i2c_states == read_addr_ack) && (tx_rx == state_wait))
    {
        //Read the ACK bit.
        OEA &= 0x7f;
        OEA |= 0x08;
        //PA6 = 0;
        PA3 = 1;
        //PA6 = 1;
        bit_count = 0x02;
        tx_rx = state_rx;
        my_i2c_states = addr_ack;
        //fast_uart(0x77,0x04);
    }
    else if ((my_i2c_states == addr_ack) && (tx_rx == state_wait))
    {
        //Find the number of data bytes to transfer and keep track
        //anl _OEA,#0xf7
        //fast_uart(tx_i2c_buffer,0x04);
        if((tx_i2c_buffer & 0x01) == 0x00)
        {
            if(rw_bit == 1)
            {

                my_i2c_states = data_read;
            }
            else
            {
                my_i2c_states = data_write;
                //fast_uart(0x88,0x04);
           }
        }
       else if((tx_i2c_buffer & 0x01) == 0x01)
        {
            if(j >= retries)
            {
                my_i2c_states = stop;
                j = 0;
            }
            else
            {
                my_i2c_states = address;
                j++;
            }
        }
//        else
//        {
//            my_i2c_states = address;
//            fast_uart(0x99,0x04);
//       }

    }
    else if ((my_i2c_states == data_read) && (tx_rx == state_wait))
    {
        //Read the data 8 bit.
        bit_count = 0x08;
        tx_rx = state_rx;
        my_i2c_states = data_read_ack;
    }
    else if ((my_i2c_states == data_read_ack) && (tx_rx == state_wait))
    {
        //Acknowledge the read data always.
        OEA |= 0xC0;
        PA6 = 0 ;
        PA7 = 0 ;
        PA6 = 1 ;
        if(i >= length)
        {
            my_i2c_states = stop;
            i = 0;
        }
        else
        {
            my_i2c_states = data_read;
            i++;
        }
    }
    else if ((my_i2c_states == data_write) && (tx_rx == state_wait))
    {
        //Send the 8 bits of data
        OEA &= 0xf7;
        OEA |= 0x80;

        tx_i2c_buffer = data[i];
        bit_count = 0x09;
        tx_rx = state_tx;
        my_i2c_states = read_data_ack;
    }
    else if ((my_i2c_states == read_data_ack) && (tx_rx == state_wait))
    {
        //Prepare for reading the data ACK bit.
        bit_count = 0x02;
        tx_rx = state_rx;
        my_i2c_states = data_write_ack;
    }
    else if ((my_i2c_states == data_write_ack) && (tx_rx == state_wait))
    {
        //Read the ACK bit for the written data
        if((tx_i2c_buffer & 0x01) == 0x01)
        {
            if(i >= length)
            {
                my_i2c_states = stop;
                i = 0;
            }
            else
            {
                my_i2c_states = data_write;
                i++;
            }
        }
        else if((tx_i2c_buffer & 0x01) == 0x00)
        {
            if(j >= retries)
            {
                my_i2c_states = stop;
                j = 0;
            }
            else
            {
                my_i2c_states = data_write;
                j++;
            }
        }

    }
    else if ((my_i2c_states == stop) && (tx_rx == state_wait))
    {
        //Send the stop bit
        OEA |= 0xC0;
        PA6 = 0 ;
        PA7 = 0 ;
        PA6 = 1 ;
        __asm
        mov r0, #0x03
        002$:djnz r0,002$
        __endasm;
        PA7 = 1 ;
        my_i2c_states = idle;
    }

}


