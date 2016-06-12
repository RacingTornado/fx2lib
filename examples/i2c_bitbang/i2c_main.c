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
#include <delay.h>
#include <fx2ints.h>
#include <i2c/i2c_utils.h>

//DELETE once merged.
void fast_uart(BYTE a);


/**************************************************
I2C declarations
***************************************************/
//Buffer to load up the data and insert into
//the i2c_client queue
__xdata unsigned char write_addr[I2C_ADDR];
//This is the data buffer.
__xdata unsigned char write_data[I2C_DATA];
//This is the address length
__xdata unsigned char rx_addr_length;
//This is the data length which needs to be read from or written to.
__xdata unsigned char rx_data_length;
//Implementation specific. Not needed for all examples. Used here
//to read and write from different address locations.
__xdata unsigned char wr_addr;


void main() {
   SETCPUFREQ(CLK_48M);
   EA = 1;			// global interrupt enable
   /********************************
   I2C BLOCK BITBANG
   *********************************/
   //Called with number of retries
   i2c_init(3);
   configure_start_timer();
   ENABLE_TIMER1();
   wr_addr = 0x03;
   while (TRUE)
    {

        write_addr[0] = 0xa0;
        write_addr[1] = 0x00;
        write_addr[2] = wr_addr;
        write_data[0] = 0x44;
        //Address, data, address length and data length
        I2CPutTX(&write_addr[0],&write_data[0],0x03,0x01);
        //i2c_control();
        write_addr[0] = 0xa1;
        write_data[0] = 0x34;
        I2CPutRXRead(&write_addr[0],0x01,0x01);
        I2CGetRXData(&write_addr[0],&write_data[0]);
        fast_uart(data[0]);
        i2c_control();
        if(wr_addr == 0x00)
        {
            wr_addr = 0x03;
        }
        else
        {
            wr_addr--;
        }
    }
}

void timer1_isr ()
__interrupt TF1_ISR
{
    __asm
    mov a,_tx_rx		//(2 cycles. Move the state into the accumulator.)
    CJNE A, #0x02, state	//(4 cycles. If in halt state, do nothing.)
    ajmp finish			//(3 cycles. Return from ISR if buffer is not ready to shift the data out.)
    state:
    djnz _bit_count,cont;	//(4 cycles. This is sued to keep track of the number of data bytes which need to be shifted out.)
    mov _tx_rx,#0x02		//(3 cycles. Set our current status to busy so nothing else can interrupt us till we shift data in or out.)
    ajmp finish			//(3 cycles. If data has been shifted, but has not been read, then jump to finish.)
    cont:
    orl _OEA,#0x40		//(3 cycles. First always set out clock pin direction. This is needed as discussed already in comments.)
    clr _PA6			//(2 cycles. Set the SCL line low.)
    mov a,_tx_rx		//(2 cycles. Move the state back into the accumulator.)
    CJNE A, #0x00, rx		//(4 cycles. Check if we are in RX or TX mode.)
    tx:
    orl _OEA,#0x80		//(3 cycles. We are in TX mode. Set the SDA direction.)
    mov a, _tx_i2c_buffer	//(2 cycles. Move the data which needs to be transmitted into the accumulator.)
    rlc a			//(1 cycles. Rotate and move the bit which needs to be sent out into the carry.)
    mov _PA7, c			//(2 cycles. Move the data into the SDA pin.)
    mov _tx_i2c_buffer,a	//(2 cycles. Now move the data back so that the next rotate can be performed when the ISR executes.)
    sjmp sclh			//(3 cycles. Jump to toggle SCL now.)
    rx:
    anl _OEA,#0x7f		//(3 cycles. Set the mode of SDA pin again.)
    mov a, _tx_i2c_buffer	//(2 cycles. Move data received into accumulator.)
    mov c,_PA7			//(2 cycles. Move the SDA line value into the carry. Delay has already been introduced between setting 					clock low and reading the first bit of data. Otherwise, follow the MPSSE i2c routine.)
    rlc a			//(1 cycle. Since the next bit will be received in the same ISR we need to shift the data.)
    mov _tx_i2c_buffer,a	//(2 cycles. Move the data back into accumulator for next ISR execution routine)
    nop				//(1 cycle. Wait for 1 cycle before setting the bit high again.) 
    sclh:
    setb _PA6			//(2 cycles. Set the SCL high)
    finish:
    nop
    __endasm;
}

/**************************
Will be DELETED once the pull requests are merged
**************************/
void fast_uart(BYTE a) {
   OEA |= 0x04;
   //An efficient UART bitbang routine in assembly
   __asm
   //Like #define in C. Can easily be used to change the pin
   .equ TX_PIN, _PA2
   //Disable interrupts
   clr _EA
   //Move the data to be sent into the ACC
   mov a , dpl
   //Clear carry
   clr c
   //We need to send out 8 bits of data
   //Load r0 with value 8
   mov r0, #0x08;
   //Create the start bit
   clr TX_PIN;
   //Precalculated delay since 1 cycle takes 88.33ns
   //Mov takes 2 cycles
   mov r1, #0x22;
   0006$:
   //DJNZ on Rn takes 3 cycle. This waits for
   //23*3, 69 cycles
   //71 cycle delay
   djnz r1, 0006$;
   //Add 2 more cycles of delay
   //97 cycles
   nop;
   nop;
   0001$:
   //2 cycles
   rrc a;
   //Move the carry into the port
   mov _PA2, c
   //Now we need to add delay for the next
   //2 cycles of delay
   mov r1, #0x1F;
   //31*3 , 93 cycles of delay
   0004$:
   djnz r1, 0004$;
   //1 more cycle, now upto 94
   nop;
   //3 more cycles of delay
   //97 cycles
   djnz r0, 0001$;
   setb _PA2;
   mov r1, #0x80;
   0005$:
   djnz r1, 0005$;
   nop
   setb _EA;
   __endasm;
}
