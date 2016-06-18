/*
Remarks By Roaring(for the FX2LP from cypress)
*/

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>
#include <fx2ints.h>


#define SYNCDELAY SYNCDELAY4

#include "softuart.h"

#define SU_TRUE    1
#define SU_FALSE   0

// startbit and stopbit parsed internally (see ISR)
#define RX_NUM_OF_BITS (8)

// 1 Startbit, 8 Databits, 1 Stopbit = 10 Bits/Frame
#define TX_NUM_OF_BITS (10)

extern void set_tx_pin_high();
//#define get_rx_pin_status()    ( SOFTUART_RXBIT )

unsigned char volatile tx_buffer;
unsigned char volatile rx_buffer;
unsigned char volatile tx_count;
unsigned char volatile rx_count;


unsigned volatile char tx_bits_sent;
unsigned volatile char rx_bits_rcvd;
unsigned char volatile tx_busy;
unsigned char volatile rx_busy;






/* Queue structure */
#define QUEUE_ELEMENTS 5
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)
__xdata unsigned char QueueTX[QUEUE_SIZE];
__xdata unsigned char QueueInTX, QueueOutTX;

__xdata unsigned char QueueRX[QUEUE_SIZE];
__xdata unsigned char QueueInRX, QueueOutRX;





void timer_init()
{

   TMOD |= 0x20;
   SYNCDELAY;
   TR1 = 0;
   SYNCDELAY;
   TH1 = 0x97;
   SYNCDELAY;
   TL1 = 0x97;
   SYNCDELAY;
}

void softuart_init( void )
{

   //io_init();
   timer_init();
   tx_busy = 0;
   rx_busy - 0;
}



void fast_uart(unsigned char a)

{
   OEA |= 0x04;
   //Multiple versions of fast UART, both of these programs are tested.
   //Check last commit for older version
   __asm
   anl _IE, #0x7f;
   mov a , dpl
   clr c
   mov r0, #0x08;
   clr _PA2;
   mov r1, #0x17;
   0006$:
   djnz r1, 0006$;
   nop;
   nop;
   0001$:
   rrc a;
   mov _PA2, c
   mov r1, #0x1F;
   0004$:
   djnz r1, 0004$;
   nop;
   djnz r0, 0001$;
   setb _PA2;
   orl IE, #0x80;
   __endasm;

}










