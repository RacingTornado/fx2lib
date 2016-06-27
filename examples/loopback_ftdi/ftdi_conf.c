#include "ftdi_conf.h"
/* -*- mode: C; c-basic-offset: 3; -*-
 *
 * convert_serial -- FX2 USB serial port converter
 *
 * by Brent Baccala <cosine@freesoft.org>   July 2009
 * adapted by Roarin
 *
 * Based on both usb-fx2-local-examples by Wolfgang Wieser (GPLv2) and
 * the Cypress FX2 examples.
 *
 * This is an FX2 program which re-enumerates to mimic a USB-attached
 * serial port (an FT8U100AX).  Anything transmitted to it by the host
 * is echoed back after converting lowercase to uppercase.  The
 * re-enumerated FX2 appears on a Debian Linux machine as something
 * like /dev/ttyUSB0, and can be tested with a serial port program
 * like 'minicom'.
 *
 * An actual FT8U100AX transmits and receives on endpoint 2.  Since
 * the FX2's endpoint 2 can be configured to either send or receive
 * (but not do both), we use endpoint 1.  This works with the Linux
 * driver, which reads the endpoint addresses from the USB
 * descriptors, but might not work with some other driver where the
 * endpoint addresses are hardwired in the driver code.
 *
 * Many features, like setting and querying things like the baudrate
 * and line/modem status are unimplemented, and will return USB errors
 * when the host attempts to perform these operations.  However, the
 * program works, and I decided to leave out the additional clutter in
 * favor of a simpler program that can be easily modified to suit
 * individual needs.
 *
 * The program ignores USB Suspend interrupts, and probably violates
 * the USB standard in this regard, as all USB devices are required to
 * support Suspend.  The remote wakeup feature is parsed and correctly
 * handled in the protocol, but of course does nothing since the
 * device never suspends.
 */
unsigned char bytes_waiting_for_xmit = 0;

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

// We want to buffer any outgoing data for a short time (40 ms) to see
// if any other data becomes available and it can all be sent
// together.  At 12 MHz we consume 83.3 ns/cycle and divide this rate
// by 12 so that our counters increment almost exactly once every us.
// The counter is sixteen bits, so we can specify latencies up to
// about 65 ms.
void process_xmit_data(void)
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

void putchar_usb(char c)
{
   __xdata unsigned char *dest=EP1INBUF + bytes_waiting_for_xmit + 2;
   // Wait (if needed) for EP1INBUF ready to accept data
   while (EP1INCS & 0x02);
   *dest = c;
   if (++bytes_waiting_for_xmit >= 1) process_xmit_data();
}

void process_recv_data(void)
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
		 {  putchar_usb(*src-'a'+'A');  }
	      else
		 {  putchar_usb(*src);  }
	   }
	EP1OUTBC=0xff; // re-arm endpoint 1 for OUT (host->device) transfers
	SYNCDELAY;
}
