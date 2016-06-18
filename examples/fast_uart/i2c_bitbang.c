/**************************************************
I2C Bitbang routines modified for FX2
Based on https://github.com/lskakaxi/mcu/blob/master/mycar/i2c-bitbang.c
***************************************************/

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

//#define I2CSPEED 400
#if (I2CSPEED==400)
# define LDELAY(); delay3us();
#else
# define LDELAY(); delay3us();
#endif

void i2c_start();
void i2c_stop();
void i2c_send_ack();
void i2c_recv_ack();
void i2c_send_byte(unsigned char data );
void delay3us();
unsigned char i2c_recv_byte();
unsigned char soft_i2c_buffer;
static unsigned char i = 0;
__bit ack_bit;
extern enum isr_state
        {
        state_tx, //0
        state_rx,  //1
        state_wait
        };
extern enum isr_state tx_rx;
extern void fast_uart(unsigned char a);
extern unsigned char bit_count;
extern unsigned char tx_i2c_buffer;




void i2c_init()
{
    tx_rx = state_wait;
}




void i2c_service()
{


    if(tx_rx == state_wait)
    {
        //fast_uart(0x20);
        bit_count = 0x08;
        tx_rx = state_tx;
        tx_i2c_buffer = 0x30;
    }
    else
    {
        //fast_uart(0x30);
        //tx_rx = state_tx;
    }


}















void i2c_start()
{
    OEA |= 0xc0;
    PA7 = 1;
    PA6 = 1;
    LDELAY();
    PA6 = 0;
    LDELAY();
    PA7 = 0;
}

void i2c_stop()
{
    OEA |= 0xc0;
    PA7 = 0;
    PA6 = 1;
    LDELAY();
    PA7 = 1;
    LDELAY();
}

void i2c_send_ack()
{
    OEA |= 0xc0;
    PA7 = 0;
    PA6 = 1;
    LDELAY();
    PA6 = 0;
    LDELAY();
}

void i2c_recv_ack()
{
    //Somewhere in the firmware the OEA bit is directly set
    //without an or. Once we find out where, then these steps
    //are not necessary
    OEA &= 0x7f;
    OEA |= 0x40;
    PA6 = 1;
    LDELAY();
    CY = PA7;
    PA6 = 0;
    LDELAY();
}

void i2c_send_byte(unsigned char data)
{
    OEA |= 0xc0;

    for (i = 0; i < 8; i++) {


        __asm
        mov a, dpl;
        rlc a;
        mov _PA7, c;
        mov dpl,a;
        __endasm;
        PA6 = 1;
        LDELAY();
        PA6 = 0;
        LDELAY();
    }
    i2c_recv_ack();
}

unsigned char i2c_recv_byte()
{
    OEA |= 0x40;
    for (i = 0; i < 8; i++) {

        PA6 = 1 ;
        LDELAY();
        __asm
        mov a, _soft_i2c_buffer;
        mov c,_PA7;
        rlc a;
        mov _soft_i2c_buffer,a;
        __endasm;
        PA6 = 0 ;
        LDELAY();


    }
    return soft_i2c_buffer;
}



void delay3us()
{
   __asm
   mov r0, #0x0b
   6900$:
   djnz r0,6900$;
   __endasm;

}


void i2c_sendByte(unsigned char addr, unsigned char data)
{
    i2c_start();
    i2c_send_byte(addr);

}
