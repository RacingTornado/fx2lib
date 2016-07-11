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

#include <lights.h>
#include <delay.h>
#include <buffer/buffer.h>
#include <stdio.h>

#define SYNCDELAY SYNCDELAY4

CREATE_BUFFER_AUTOPTR_SINGLE(buffer0,10)
CREATE_BUFFER_AUTOPTR_SINGLE(buffer1,10)
CREATE_BUFFER_AUTOPTR_SINGLE(buffer2,5)
__xdata BYTE data_buffer[10];
void uart0_tx(char data);
void main(void)
{
	buffer0_init();
	buffer1_init();
	buffer2_init();
	SETCPUFREQ(CLK_48M);
	// loop endlessly
	for(;;) {
		//printf("HeaderWrite0 MSB %02x \r\n",head_MSB);
		//printf("HeaderWrite0 LSB %02x \r\n",head_LSB);
		//printf("Buffer 0 source %02x \r\n",buffer0src);
		//printf("Buffer 0 offser %02x \r\n",buffer0_offset);
		//printf("Buffer 1 source %02x \r\n",buffer1src);
		//printf("Buffer 1 offser %02x \r\n",buffer1_offset);
		//printf("Buffer0 is at %p \r\n",buffer0_buffer);
		//printf("HeaderRead0 MSB %02x \r\n",tail_MSB);
		//printf("HeaderRead0 LSB,now pushing %02x \r\n",tail_LSB);
//printf("Buffer number %02x \r\n",buffer_number);
		buffer0_push(0x45);
		buffer0_push(0x46);
		buffer0_push(0x47);
		buffer0_push(0x48);
		buffer2_push(0x21);
		//buffer0_push(0x49);
		//buffer0_push(0x50);
		//printf("HeaderWrite0 MSB,now pushed %02x \r\n",head_MSB);
		//printf("HeaderWrite0 LSB %02x \r\n",head_LSB);
		//printf("Buffer 0 source %02x \r\n",buffer0src);
		//printf("Buffer 0 offser %02x \r\n",buffer0_offset);
		//printf("Buffer 1 source %02x \r\n",buffer1src);
		//printf("Buffer 1 offser %02x \r\n",buffer1_offset);
		//printf("Buffer0 is at %p \r\n",buffer0_buffer);
		//printf("HeaderRead0 MSB %02x \r\n",tail_MSB);
		//printf("HeaderRead0 LSB %02x \r\n",tail_LSB);
		buffer1_push(0x35);
		buffer1_push(0x36);
		buffer1_push(0x37);
		buffer1_push(0x38);
		buffer1_push(0x39);
		buffer1_push(0x40);
		//printf("HeaderWrite1 MSB %02x \r\n",head_MSB);
		//printf("HeaderWrite1 LSB %02x \r\n",head_LSB);
		//printf("Buffer1 is at %p \r\n",buffer1_buffer);
		//printf("HeaderRead1 MSB %02x \r\n",tail_MSB);
		//printf("HeaderRead1 LSB %02x \r\n",tail_LSB);
printf("Popping data\r\n");
//printf("Current buffer pop %02x \r\n",current_buffer_pop);
//printf("buffer1 number pop %02x \r\n",buffer0number);
printf("Buffer 0 %02x\r\n",buffer0_pop());
printf("Buffer 0 %02x\r\n",buffer0_pop());
printf("Buffer 0 %02x\r\n",buffer0_pop());
printf("Buffer 0 %02x\r\n",buffer0_pop());
//		printf("HeaderRead0 MSB %02x \r\n",tail_MSB);
//		printf("HeaderRead0 LSB %02x \r\n",tail_LSB);
//printf("Data is %02x\r\n",buffer0_pop());
//printf("Data is %02x\r\n",buffer0_pop());
//printf("Data is %02x\r\n",buffer0_pop());
//printf("Data is %02x\r\n",buffer0_pop());
//printf("Data is %02x\r\n",buffer0_pop());

printf("Buffer 1 %02x\r\n",buffer1_pop());
printf("Buffer 1 %02x\r\n",buffer1_pop());
printf("Buffer 1 %02x\r\n",buffer1_pop());
printf("Buffer 1 %02x\r\n",buffer1_pop());
printf("Buffer 1 %02x\r\n",buffer1_pop());
printf("Buffer 1 %02x\r\n",buffer1_pop());
printf("Buffer 2 %02x\r\n",buffer2_pop());
	    }
}

void uart0_tx(char c)
{
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
    clr _EA //(2 cycles)
    //Move the data to be sent into the ACC
    //The data which is to be shifted out is held in the dpl register
    //We move the data into A for easy access to subsequent instructions
    mov a , dpl //(2 cyles)
    clr c //(1 cycle)
    //We need to send out 8 bits of data
    //Load r0 with value 8
    mov r0, #0x08 //(2 cycles)
    //Create the start bit
    clr _TX_PIN  //(2 cycles)
    //Precalculated delay since 1 cycle takes 88ns
    //At 12Mhz, it should be about 83.33ns
    //But it appears to be about 88ns
    //These numbers have been verified using an analyzer
    mov r1, #0x20 //(2 cycles)
    0006$:
    //1 bit is about 8.6us
    djnz r1, 0006$ //DJNZ on Rn takes (3 cycles)
    //NOP takes about 1 cycle
    //Add 2 more cycles of delay
    //97 cycles
    nop //(1 cycle)
    nop //(1 cycle)
    0001$:
    rrc a // (2 cycles). This rotates the accumulator right through the carry
    //Move the carry into the port
    mov _TX_PIN, c //(2 cycles)
    //Now we need to add delay for the next
    mov r1, #0x1F //(2 cycles) 
    //31*3 , 93 cycles of delay
    0004$:
    djnz r1, 0004$ //(3 cycles)
    nop  //(1 cycle)
    //3 more cycles of delay
    //97 cycles
    djnz r0, 0001$ //(3 cycles)
    setb _TX_PIN  //(2 cycles) This is for stop bit
    //We need to delay the stop bit,  otherwise we may get errors.
    mov r1, #0x20 //(2 cycles)
    0005$:
    djnz r1, 0005$ //(3 cycles) for DJNZ , Jump for 32*3 , 96 cycles 
    nop //(NOP takes 1 cycle) 97 cycles of delay
    setb _EA; //Enable back the interrupts
    __endasm;
}

void putchar(char c)
{
uart0_tx(c);
}









