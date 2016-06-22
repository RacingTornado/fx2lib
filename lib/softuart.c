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
#define QUEUE_ELEMENTS 50
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)
__xdata unsigned char QueueTX[QUEUE_SIZE];
__xdata unsigned char QueueInTX, QueueOutTX;
__xdata unsigned char QueueRX[QUEUE_SIZE];
__xdata unsigned char QueueInRX, QueueOutRX;



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
   tx_busy = 0;
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
      if ( tx_busy == 0 )
      {
         //Load value
         QueueGetTX(&tx_buffer);
         //Busy. Operation is ongoing
         tx_busy = 1;
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




void uart0_transmit(char c)
{
    //Put the data into the buffer.
    QueuePutTX(c);
}


void process_isr()
{

   tx_count = tx_count + 1;
   if ( (tx_count % 4)  == 0)
   {
                 __asm
   mov _OEA, #0x08
   cpl _PA3
   __endasm;
      if (tx_busy == 0x01)
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
            tx_busy = 0;
         }
      }

      tx_count = 0x00;
   }

   rx_count = rx_count + 1;

   if (rx_busy == 0x00)
   {
      __asm
      anl _OEA, #0xdf;
      mov c, _PA5;
      jc 0001$;
      mov _rx_count, #0x00
      mov _rx_bits_rcvd, #0x00
      mov _rx_busy , #0x02
      0001$:
      __endasm;
   }


   if ( (rx_count % 4)  == 0)
   {
      if ((rx_busy == 0x02) || (rx_busy == 0x03))
      {
         rx_busy = 0x03;
         OEA &= 0xdf;
         rx_bits_rcvd ++;

         //Writing bits out via UART
         if (rx_bits_rcvd < 10)
         {
            __asm
            mov a, _rx_buffer;
            mov c, _PA5;
            rrc a;
            mov _rx_buffer, a;
            __endasm;
         }
         else
         {
            __asm
            mov c, _PA5;
            __endasm;
            rx_bits_rcvd = 0;
            rx_busy = 1;
         }
      }

      rx_count = 0x00;
   }

}


void fast_uart(unsigned char a)
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
