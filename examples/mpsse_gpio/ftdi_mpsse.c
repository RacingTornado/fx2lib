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
#include "mpsse_utils.h"
#include <fx2ints.h>
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
volatile __bit got_ep1_in;
unsigned char isr_enter;
unsigned short counter ;
unsigned short delete_length;
unsigned long delete_total_count;
unsigned char volatile mpsse_bit_count;

void main()
{
    //Setup data available and other init













    got_sud=FALSE;
    //Call our custom function to do our UART init
    configure_endpoints();
    mpsse_configure_timer();
    RENUMERATE();
    SETCPUFREQ(CLK_48M);
    //Enable USB auto vectored interrupts
    USE_USB_INTS();
    ENABLE_SUDAV();
    ENABLE_SOF();
    ENABLE_HISPEED();
    ENABLE_USBRESET();
    ENABLE_EP2();
    //Do we really need this??
    //ENABLE_EP1IN();
    ENABLE_TIMER1();
     // arm ep2
 EP2BCL = 0x80; // write once
 SYNCDELAY;
 EP2BCL = 0x80; // do it again
    EA=1; // global interrupt enable
	REVCTL=0x00;
	SYNCDELAY;
	isr_enter = 0;






    while(TRUE)
    {
        //Handles device descriptor requests

        if ( got_sud )
        {
            handle_setupdata();
            got_sud=FALSE;
        }
        if ( isr_enter != 0 )
        {
            /* Data from the host to the device*/
            delete_length= EP2BCL | (EP2BCH <<8);
            printf("Got data %02x, length is %04x %02x",EP2FIFOBUF[6], EP2BCL | (EP2BCH <<8),EP1INCS);
            printf("[ ");
            for(counter =0 ;counter < delete_length;counter++)
            {
               printf("%02x-%02d, ",EP2FIFOBUF[counter],counter);
            }
            printf("]\r\n");
            /*Handle the bulk data*/
            mpsse_handle_bulk();
            delete_total_count = delete_length +delete_total_count;
            printf("TOTAL length main %lu",delete_total_count);
            flush_ep1in_data();
            /* Rearm the EP.*/
             // arm ep2
            EP2BCL = 0x80; // write once
            EP1INCS = 0x00;
            got_ep2=FALSE;
            isr_enter -- ;
        }
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

/*Handles the ISR for data on EP 0x01*/
void ep1in_isr() __interrupt EP1IN_ISR
{
    got_ep1_in = TRUE;
    CLEAR_EP1IN();
}

/*Handles the ISR for data on EP 0x02*/
void ep2_isr()
__interrupt EP2_ISR
{
    got_ep2 = TRUE;
    isr_enter++;
    CLEAR_EP2();
}

void timer1_isr ()
__interrupt TF1_ISR
{
    /* This routine will always clock bit_count bits. Data is set when
     * clk is low. It will clock out data MSB first.
     */
    __asm
    mov a,_isr_state      //Find the current state
    cjne a, #0x02, state  //If COMPLETE, then wait for buffers to be read out.
    ajmp finish
    state:
    cjne a, #0x00, start_op
    ajmp finish
    start_op:
    mov r0,_mpsse_bit_count;
    djnz r0,cont;
    mov _mpsse_bit_count,r0;
    mov _isr_state,#0x02
    ajmp finish
    cont:
    mov _mpsse_bit_count,r0;
    clr _PA0 //Clear the TCK pin
    mov a,_isr_mode
    CJNE A, #0x00, rx
    tx:
    mov a, _mpsse_isr_buffer;
    rlc a;
    mov _PA1, c //PA1 is DO(see PINMAPPING docs)
    mov _mpsse_isr_buffer,a;
    sjmp clkh
    rx:
    mul ab
    mul ab
    setb _PA0 //Set SCL
    mul ab
    mov a, _mpsse_isr_buffer;
    mov c,_PA1 //PA2(connected to PA1 for I2C) is DI(see PINMAPPING docs)
    rlc a;
    mov _mpsse_isr_buffer,a;
    sjmp finish
    clkh:
    setb _PA0 //PA0 is the TCK pin(see PINMAPPING docs)
    finish:
    nop
    __endasm;


}














