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
#include <fx2macros.h>
#include <autovector.h>
#include <setupdat.h>
#include <eputils.h>
#include <delay.h>
#include "ftdi_conf.h"
#define DEBUG_MAIN
#ifdef DEBUG_MAIN
#include <stdio.h> // NOTE this needs deleted
#else
#define printf(...)
#endif

void uart_tx(char c);
//For handling SUDAV ISR
volatile __bit got_sud;
volatile __bit got_ep2;

void main()
{
    //Setup data available and other init
    got_sud=FALSE;
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
    ENABLE_EP2();
    EP2BCL = 0xff;
    EA=1; // global interrupt enable

    while(TRUE)
    {
        //Handles device descriptor requests
        if ( got_sud )
        {
            handle_setupdata();
            got_sud=FALSE;
        }
        if ( got_ep2 )
        {
            /* Data from the host to the device*/
            printf("Got data\r\n");
            mpsse_handle_bulk();
            got_ep2=FALSE;
            EP2BCL = 0xff;
        }
        // Input data on EP1
	    //putchar(0x23);
    }
}

BOOL
handle_get_descriptor ()
{
    return FALSE;
}

BOOL
handle_vendorcommand (BYTE cmd)
{
    //printf ("Need to implement vendor command: %02x\n", cmd);
    mpsse_handle_control();
    return FALSE;
}

// this firmware only supports 0,0
BOOL
handle_get_interface (BYTE ifc, BYTE * alt_ifc)
{
    //printf ("Get Interface\n");

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
    //printf ("Set interface %d to alt: %d\n", ifc, alt_ifc);

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


void ep2_isr()
__interrupt EP2_ISR
{
    got_ep2 = TRUE;
    CLEAR_EP2();
}















