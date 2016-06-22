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
   ENABLE_TIMER1();
}

void softuart_init( void )
{

   timer_init();
   QueueInitRX();
   QueueInitTX();
   tx_busy = 0;
   rx_busy - 0;
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
