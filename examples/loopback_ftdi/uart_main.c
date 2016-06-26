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
#ifdef DEBUG_MAIN
#include <stdio.h> // NOTE this needs deleted
#else
#define printf(...)
#endif

#include <fx2regs.h>
#include <fx2macros.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>


#define SYNCDELAY SYNCDELAY4
#define REARMVAL 0x80
#define REARM() EP2BCL=REARMVAL
#define FTDI_RS0_CTS    (1 << 4)
#define FTDI_RS0_DSR    (1 << 5)
#define FTDI_RS0_RI     (1 << 6)
#define FTDI_RS0_RLSD   (1 << 7)

#define FTDI_RS_DR  1
#define FTDI_RS_OE (1<<1)
#define FTDI_RS_PE (1<<2)
#define FTDI_RS_FE (1<<3)
#define FTDI_RS_BI (1<<4)
#define FTDI_RS_THRE (1<<5)
#define FTDI_RS_TEMT (1<<6)
#define FTDI_RS_FIFO  (1<<7)




volatile WORD bytes;
volatile __bit gotbuf;
volatile BYTE icount;
volatile __bit got_sud;
DWORD lcount;
__bit on;


void main() {


    //Setup data available and other init
    got_sud=FALSE;
    gotbuf=FALSE;
    bytes=0;
    //Call our custom function to do our UART init
    configure_endpoints();
    RENUMERATE();
    SETCPUFREQ(CLK_48M);
    //Enable USB auto vectored interrupts
    USE_USB_INTS();
    ENABLE_SUDAV();
    ENABLE_SOF();
    ENABLE_HISPEED();
    ENABLE_USBRESET();
    EA=1; // global interrupt enable
    while(TRUE) {
        //Handles device descriptor requests
        if ( got_sud ) {
        handle_setupdata();
        got_sud=FALSE;
        }
        // Input data on EP1
        if(!(EP1OUTCS & bmEPBUSY))
        {
           ProcessRecvData();
        }
 }

}

void configure_endpoints(void)
{

	REVCTL=0x03;  // See TRM...
	SYNCDELAY;
	// Endpoint configuration - everything disabled except
	// bidirectional transfers on EP1.
	EP1OUTCFG=0xa0;
	EP1INCFG=0xa0;
	EP2CFG=0;
	EP4CFG=0;
	EP6CFG=0;
	EP8CFG=0;
	SYNCDELAY;
	EP1OUTBC=0xff; // Arm endpoint 1 for OUT (host->device) transfers

}

// These defines came from the Linux source code:
//    drivers/usb/serial/ftdi_sio.h
//
// That file also does a reasonable job of documenting the protocol,
// particularly the vendor requests I don't implement that do things
// like setting the baud rate.

#define FTDI_RS0_CTS    (1 << 4)
#define FTDI_RS0_DSR    (1 << 5)
#define FTDI_RS0_RI     (1 << 6)
#define FTDI_RS0_RLSD   (1 << 7)

#define FTDI_RS_DR  1
#define FTDI_RS_OE (1<<1)
#define FTDI_RS_PE (1<<2)
#define FTDI_RS_FE (1<<3)
#define FTDI_RS_BI (1<<4)
#define FTDI_RS_THRE (1<<5)
#define FTDI_RS_TEMT (1<<6)
#define FTDI_RS_FIFO  (1<<7)

// We want to buffer any outgoing data for a short time (40 ms) to see
// if any other data becomes available and it can all be sent
// together.  At 12 MHz we consume 83.3 ns/cycle and divide this rate
// by 12 so that our counters increment almost exactly once every us.
// The counter is sixteen bits, so we can specify latencies up to
// about 65 ms.

unsigned int bytes_waiting_for_xmit = 0;

static void ProcessXmitData(void)
{
	// Lead in two bytes in the returned data (modem status and
	// line status).
	EP1INBUF[0] = FTDI_RS0_CTS | FTDI_RS0_DSR | 1;
	EP1INBUF[1] = FTDI_RS_DR;
	// Send the packet.
	SYNCDELAY;
	EP1INBC = bytes_waiting_for_xmit + 2;
	bytes_waiting_for_xmit = 0;
}


static void putchar(char c)
{
   __xdata unsigned char *dest=EP1INBUF + bytes_waiting_for_xmit + 2;
   // Wait (if needed) for EP1INBUF ready to accept data
   while (EP1INCS & 0x02);
   *dest = c;
   if (++bytes_waiting_for_xmit >= 1) ProcessXmitData();
}

static void ProcessRecvData(void)
{
	__xdata const unsigned char *src=EP1OUTBUF;
	unsigned int len = EP1OUTBC;
	unsigned int i;
	// Skip the first byte in the received data (it's a port
	// identifier and length).
	src++; len--;
	for(i=0; i<len; i++,src++)
	   {
	      if(*src>='a' && *src<='z')
		 {  putchar(*src-'a'+'A');  }
	      else
		 {  putchar(*src);  }
	   }
	EP1OUTBC=0xff; // re-arm endpoint 1 for OUT (host->device) transfers
	SYNCDELAY;
}


void main1(void)
{
	// Disconnect the USB interface, initialize, renumerate, reconnect
	USBCS |= 0x08;
	//Initialize();
	USBCS |= 0x02;
	USBCS &= ~(0x08);
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




void
sof_isr ()
__interrupt SOF_ISR __using 1
{
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
