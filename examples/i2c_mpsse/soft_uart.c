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



static void io_init(void)
{
   // TX-Pin as output
   SOFTUART_TXDDR |=  ( 1 << SOFTUART_TXBITNO );
   // RX-Pin as input
   SOFTUART_RXDDR &= ~( 1 << SOFTUART_RXBITNO );
}

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
  
   io_init();
   timer_init();
   QueueInitRX();
   QueueInitTX();
   tx_busy = 0;
   rx_busy - 0;
}



unsigned char fast_uart(unsigned char a)

{
   OEA |= 0x04;
   //Multiple versions of fast UART, both of these programs are tested.
   // I should probably remove them from this commit, but I thought it would be good to have only this
   // In case we may need to use the fast_uart using the rr operations. 
   //    __asm
   //    anl IE, #0x7f;
   //    mov a ,dpl
   //    clr c
   //    mov r0, #0x08;
   //    clr _PA2;
   //    0005$:
   //    mov r1, #0x17;
   //    0006$:
   //    djnz r1,0006$;
   //    nop;
   //    nop;
   //    0001$:
   //    rrc a;
   //    jc 0002$;
   //    clr _PA2;
   //    mov r1,#0x1e;
   //    0008$:
   //    djnz r1, 0008$
   //    nop;
   //
   //
   //
   //    ajmp 0007$;
   //    0002$:
   //    setb _PA2;
   //    0003$:
   //    mov r1, #0x1f;
   //    0004$:
   //    djnz r1, 0004$;
   //    nop;
   //    0007$:
   //    djnz r0, 0001$;
   //    setb _PA2;
   //    orl IE, #0x80;
   //
   //    __endasm;
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
   a=20+a;
   return a;
}



//This function is called periodically from main
void uart_tx_service()
{
   //Data has been loaded by the calling function in a buffer
   //We need to move the data and set the flag
   //Check if operation is ongoing
   if ( QueueCheckTX() != 1)
   {
      if ( tx_busy == 0 )
      {
         //fast_uart(tx_count);
         //Load value
         QueueGetTX(&tx_buffer);
         //Busy. Operation is ongoing
         //Clear bit 7  indicates that operation has completed.
         tx_busy = 1;
      }
   }
}


//Called periodically from main
void uart_rx_service()
{
   //Will be replaced with enum in next version
   // 0x00 - IDLE
   // 0x01 - Data Reception complete
   // 0x02 - Start bit detect
   // 0x03 - Data currently being read
   if (rx_busy == 0x01 )
   {
      //Load value
      QueuePutTX(rx_buffer);
      rx_busy = 0x00;
   }
}


//Code taken from StackOverflow for Queue implementation

/* Very simple queue
 * These are FIFO queues which discard the new data when full.
 *
 * Queue is empty when in == out.
 * If in != out, then
 *  - items are placed into in before incrementing in
 *  - items are removed from out before incrementing out
 * Queue is full when in == (out-1 + QUEUE_SIZE) % QUEUE_SIZE;
 *
 * The queue will hold QUEUE_ELEMENTS number of items before the
 * calls to QueuePut fail.
 */



void QueueInitTX(void)
{
   QueueInTX = QueueOutTX = 0;
}

__bit QueuePutTX(unsigned char data)
{
   if (QueueInTX == (( QueueOutTX - 1 + QUEUE_SIZE) % QUEUE_SIZE))
   {
      return 1; /* Queue Full*/
   }

   QueueTX[QueueInTX] = data;
   QueueInTX = (QueueInTX + 1) % QUEUE_SIZE;
   return 0; // No errors
}

__bit QueueGetTX(unsigned char * old)
{
   if ((QueueInTX == QueueOutTX))
   {
      return 1; /* Queue Empty - nothing to get*/
   }

   *old = QueueTX[QueueOutTX];
   QueueOutTX = (QueueOutTX + 1) % QUEUE_SIZE;
   return 0; // No errors
}


__bit QueueCheckTX()
{
   //fast_uart(QueueInTX);
   //        fast_uart(QueueOutTX);
   if ((QueueInTX == QueueOutTX))
   {
      return 1; /* Queue Empty - nothing to get*/
   }

   return 0; // No errors
}

void QueueInitRX(void)
{
   QueueInRX = QueueOutRX = 0;
}

__bit QueuePutRX(unsigned char data)
{
   if (QueueInRX == (( QueueOutRX - 1 + QUEUE_SIZE) % QUEUE_SIZE))
   {
      return 1; /* Queue Full*/
   }

   QueueRX[QueueInRX] = data;
   QueueInRX = (QueueInRX + 1) % QUEUE_SIZE;
   return 0; // No errors
}

__bit QueueGetRX(unsigned char * old)
{
   if (QueueInRX == QueueOutRX)
   {
      return 1; /* Queue Empty - nothing to get*/
   }

   *old = QueueRX[QueueOutRX];
   QueueOutRX = (QueueOutRX + 1) % QUEUE_SIZE;
   return 0; // No errors
}

//This function checks if Queue is full . If full, it returns 1
__bit QueueCheckRX()
{
   if (QueueInRX == (( QueueOutRX - 1 + QUEUE_SIZE) % QUEUE_SIZE))
   {
      return 1; /* Queue Full*/
   }

   return 0; // No errors
}

