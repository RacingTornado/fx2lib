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


unsigned char volatile tx_buffer;
unsigned char volatile rx_buffer;
unsigned char volatile tx_count;
unsigned char volatile rx_count;
unsigned volatile char tx_bits_sent;
unsigned volatile char rx_bits_rcvd;
unsigned char volatile rx_busy;






/* Queue structure */
#define QUEUE_ELEMENTS 50
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)
__xdata unsigned char QueueTX[QUEUE_SIZE];
__xdata unsigned char QueueInTX, QueueOutTX;
__xdata unsigned char QueueRX[QUEUE_SIZE];
__xdata unsigned char QueueInRX, QueueOutRX;
/**
 * Enum for controlling transmitter state
**/
enum uart_tx_state tx_uart_state;


void uart0_init()
{

    timer_init();
    softuart_init();
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
   ENABLE_TIMER1();
   EA = 1;
   TR1 = 1;
}

void softuart_init( void )
{

   timer_init();
   QueueInitRX();
   QueueInitTX();
   CPUCS = 0x10;
   tx_uart_state = IDLE;
   rx_busy - 0;
}

/**
 * \brief Called periodically from main. If the queue is non-empty
 * then it returns immediately(non blocking). If there is data, it sets
 * up the ISR to transmit this data and then returns.
**/
void uart_tx_service()
{
   //Data has been loaded by the calling function in a buffer
   //Check if operation is ongoing
   //fast_uart(tx_buffer);
   if ( QueueCheckTX() != 1)
   {
      if ( tx_uart_state == IDLE )
      {
         //Load value
         QueueGetTX(&tx_buffer);
         //Busy. Operation is ongoing
         tx_uart_state = BUSY;
         tx_count = 0;
      }
   }
}


/**
 * \brief Called periodically from main. If the data reception is
 * complete, then the rx_buffer is automatically put into
 * queue , if the queue is non empty.
**/
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
 * Code taken from StackOverflow.com
 */

/**
 * Initialize the TX queue
 **/
void QueueInitTX(void)
{
   QueueInTX = QueueOutTX = 0;
}


/**
 * \brief Insert data into the TX queue
 *
 * \param data Data to be printed out on UART
 **/
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


/**
 * \brief This is called to get data from the TX queue
 *
 * \param old the data which is returned(pointer)
 **/
__bit QueueGetTX(unsigned char  *old)
{
   if ((QueueInTX == QueueOutTX))
   {
      return 1; /* Queue Empty - nothing to get*/
   }

   *old = QueueTX[QueueOutTX];
   QueueOutTX = (QueueOutTX + 1) % QUEUE_SIZE;
   return 0; // No errors
}


/**
 * \brief Check if the tx_queue is full
 **/
__bit QueueCheckTX()
{
   if ((QueueInTX == QueueOutTX))
   {
      return 1; /* Queue Empty - nothing to get*/
   }

   return 0; // No errors
}


/**
 * \brief Initialize the rx_queue
 *
 **/
void QueueInitRX(void)
{
   QueueInRX = QueueOutRX = 0;
}


/**
 * \brief Insert data into the RX queue
 *
 * \param data Data obtained from UART
 **/
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


/**
 * \brief Get data from the rx_queue
 *
 * \param old Pointer to the data which is loaded when this function is called
 **/
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



/**
 * \brief This function checks if Queue is full . If full, it returns 1
 **/
__bit QueueCheckRX()
{
   if (QueueInRX == (( QueueOutRX - 1 + QUEUE_SIZE) % QUEUE_SIZE))
   {
      return 1; /* Queue Full*/
   }

   return 0; // No errors
}


/**
 * \brief This function is called from putchar
 **/
void uart0_transmit(char c)
{
    //Put the data into the buffer.
    QueuePutTX(c);
}

/**
 * \brief This function is called from timer 1 ISR
 * Optimized for use in RX and TX
 **/
void process_isr()
{

   tx_count = tx_count + 1;
   if ( (tx_count % 4)  == 0)
   {
                 __asm
   mov _OEA, #0x08
   cpl _PA3
   __endasm;
      if (tx_uart_state == BUSY)
      {
         OEA |= 0x10;
         tx_bits_sent ++;

         //Writing bits out via UART
         if (tx_bits_sent == 1)
         {
            PA4 = 0 ;
         }
         else if (tx_bits_sent > 1 && tx_bits_sent <= 10)
         {
            __asm
            mov a, _tx_buffer;
            rrc a;
            mov _PA4, c;
            mov _tx_buffer, a;
            __endasm;
         }
         else
         {
            PA4 = 1;
            tx_bits_sent = 0;
            tx_uart_state = IDLE;
         }
      }

      tx_count = 0x00;
   }
}
