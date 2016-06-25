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
#include <mpsse/mpsse_utils.h>
#include <fx2ints.h>
#define DEBUG_MAIN
#ifdef DEBUG_MAIN
#include <stdio.h> // NOTE this needs deleted
#else
#define printf(...)
#endif

#define SYNCDELAY SYNCDELAY4
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
    ENABLE_TIMER1();
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
            /*Handle the bulk data*/
            mpsse_handle_bulk();
            got_ep2=FALSE;
            /* Rearm the EP.*/
            EP2BCL = 0xff;
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

/*Handles the ISR for data on EP 0x02*/
void ep2_isr()
__interrupt EP2_ISR
{
    got_ep2 = TRUE;
    CLEAR_EP2();
}

/*********************************************************************************
DELETE THIS
**********************************************************************************/
unsigned char load_delay;
void uart_tx(char c)
{

    load_delay = 0x20;
    //Done in ASM to improve performance. It takes only 6
    //cycles to move the data out, however a delay has been
    //introduced in order to get a baud rate of 115200
    //The mask which is to be written into the pin
    OEA |= 0x04;
    //An efficient UART bitbang routine in assembly
    __asm
    //Like #define in C. Can easily be used to change the pin
    .equ _TX_PIN, _PA2
    //Disable interrupts
    //This is used because timing is critical
    //If the FX2 jumps into the ISR temporarily , it may cause transmit
    //errors. By clearing EA, we can disable interrupts
    clr _EA             //(2 cycles)
    //Move the data to be sent into the ACC
    //The data which is to be shifted out is held in the dpl register
    //We move the data into A for easy access to subsequent instructions
    mov a , dpl         //(2 cyles)
    clr c               //(1 cycle )
    //We need to send out 8 bits of data
    //Load r0 with value 8
    mov r0, #0x08       //(2 cycles)
    //Create the start bit
    clr _TX_PIN         //(2 cycles)
    //Precalculated delay since 1 cycle takes 88ns
    //At 12Mhz, it should be about 83.33ns
    //But it appears to be about 88ns
    //These numbers have been verified using an analyzer
    mov r1, _load_delay //(2 cycles)
    0006$:
    //1 bit is about 8.6us
    djnz r1, 0006$      //(3 cycles)
    //DJNZ on Rn takes 3 cycles
    //NOP takes about 1 cycle
    //Add 2 more cycles of delay
    //97 cycles
    nop                 //(1 cycle )
    nop                 //(1 cycle )
    0001$:
    rrc a               //(2 cycles)
    //The above rotates the accumulator right through the carry
    //Move the carry into the port
    mov _TX_PIN, c      //(2 cycles)
    //Now we need to add delay for the next
    mov r1, _load_delay //(2 cycles)
    //31*3 , 93 cycles of delay
    0004$:
    djnz r1, 0004$      //(3 cycles)
    nop                 //(1 cycle )
    //3 more cycles of delay
    //97 cycles
    djnz r0, 0001$      //(3 cycles)
    setb _TX_PIN        //(2 cycles)
    //This is for stop bit
    //We need to delay the stop bit,  otherwise we may get errors.
    mov r1, _load_delay //(2 cycles)
    0005$:
    djnz r1, 0005$      //(3 cycles)
    //for DJNZ , Jump for 32*3 , 96 cycles
    nop                 //(1 cycle )
    //97 cycles of delay
    setb _EA            //(2 cycles)
    //Enable back the interrupts
    __endasm;
}

void putchar(char c)
{
    uart_tx(c);
}
