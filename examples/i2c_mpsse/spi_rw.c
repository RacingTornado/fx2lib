#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>

//unsigned char spi_rcv;
//unsigned char shold_data;
//unsigned char sinternal_hold_data;
//unsigned char sptrx;

unsigned char new_data;
unsigned char new_master_pin;



void spi_data_logic(unsigned char mosi_data,unsigned  char master_pin)
{

	new_data = mosi_data;
	//new_master_pin = master_pin;
	OEA = master_pin;// Change to OR later

	__asm
	anl _IE, #0x7f
	setb _PA3;
	nop
	mov r1, #08
	mov a, _new_data
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
	setb _PA3;
	__endasm;


}

void spi_mosi_data_logic()
{

	__asm
	anl _IE, #0x7f
	orl _OEA,#0x2C; enabled three pins 5(clock) and 3(MOSI)
	setb _PA3;
	nop
	mov r1, #08
	mov a , #0x12
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
	setb _PA3;
	orl _IE,#0x80
	__endasm;


}



void spi_miso_data_logic()
{

	__asm

	anl _IE, #0x7f
	mov _OEA,#0x64; enabled three pins 5(clock) and 6(MISO)
	setb  _PA6;
	nop
	mov r1, #08
	mov a , #0x34
	0001$:
	rlc a;
	clr _PA5;
	jc 0003$;
	clr  _PA6;
	ljmp 0002$;
	0003$:
	setb _PA6;
	ljmp 0002$; added
	0002$:
	setb _PA5;
	djnz r1, 0001$
	setb _PA6;
	orl _IE,#0x80
	__endasm;

}
