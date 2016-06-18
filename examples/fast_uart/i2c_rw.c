#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>

unsigned char i2c_rcv;
unsigned char hold_data;
unsigned char internal_hold_data;
unsigned char ptrx;

void i2c_addr_logic()
{



	i2c_rcv = 'e';
	if(i2c_rcv == 's')
	return;
	__asm
	anl _IE, #0x7f
	orl _OEA,#0x28; enabled three pins 5(SCL) and 3
	mov r1, #09
	mov a , #44
	clr  _PA3;
	nop
	0001$:
	rlc a;
	clr _PA5;
	jc 0003$;
	clr  _PA3;
	ljmp 0002$;
	0003$:
	setb _PA3;
	ljmp 0002$; added
	0002$:
	setb _PA5;
	djnz r1, 0001$
	orl _IE,#0x80
	;setb _PA3; this is for stop bit and has to be tackled later
	clr _PA5;
	__endasm;


}



void i2c_data_logic(unsigned char portenselect)
{

	__asm
	mov _hold_data , a
	mov _internal_hold_data, b;
	anl _IE, #0x7f
	orl _OEA,#0x68; enabled pin 6,5 and 3
	anl _OEA,#0xf7; enabled pins 6,5(SCL) disabled pin3
	mov r1, #08
	mov a , #57
	mov r2, #00
	0001$:
	rlc a;
	clr _PA5;
	jc 0002$;
	clr  _PA6;
	ljmp 0003$;
	0002$:
	setb _PA6;
	ljmp 0003$; added
	0003$:
	mov _ptrx , a
	mov a, r2;
	rl a;
	setb _PA5;
	mov C, _PA3;
	mov a.0, C;
	mov r2, a;
	mov a, _ptrx;
	djnz r1, 0001$
	orl _OEA,#0x68; enabled pin 6,5 and 3
	anl _OEA,#0xbf; enabled pins 3,5(SCL) disabled pin6
	0004$:
	clr _PA5;
	jc 0005$;
	clr  _PA3;
	ljmp 0006$;
	0005$:
	setb _PA3;
	ljmp 0006$; added
	0006$:
	nop
	nop
	nop
	setb _PA5;
	mov C, 0;dummy
	mov C, 0;dummy
	mov C, 0;dummy
	mov C, 0;dummy
	mov C, _PA6;
	clr _PA5;
	orl _IE,#0x80
	mov dpl, r2;
	mov a, _hold_data;
	mov b, _internal_hold_data;
	__endasm;

}


void i2c_stop_logic()
{
	__asm
	clr  _PA3;
	nop
	nop
	setb _PA5;
	nop
	nop
	nop
	setb _PA3; this is for stop bit
	__endasm;

}


void temp_call()
{
	__asm
	cpl a
	cpl a
	__endasm;

}

char xxy(char a, char b)
{
	char d;
	b = a|b|d;
	return b;


}



