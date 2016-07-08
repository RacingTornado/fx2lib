/** \file lib/uart/soft_uart.c
 * This file is for defining a common API for accessing UARTs.
 * Remarks By Roaring(for the FX2LP from cypress)
 **/
#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <uart/api.h>
#include <delay.h>
#include <uart/timer_uart.h>
#include <fx2ints.h>
#define SYNCDELAY SYNCDELAY4
/**
 * tx_buffer holds the byte which is being transmitted out
**/
unsigned char volatile tx_buffer;
/**
 * rx_buffer holds the byte which is being received
**/
unsigned char volatile rx_buffer;
/**
 * tx_count holds the number of bits in tx_buffer which have been
 * sent out
**/
unsigned char volatile tx_count;
/**
 * rx_count holds the number of bits in rx_buffer which have been
 * moved in
**/
unsigned char volatile rx_count;
unsigned volatile char tx_bits_sent;
unsigned volatile char rx_bits_rcvd;
/* Queue structure */
#define QUEUE_ELEMENTS 50
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)
/* QueueTX is the transmit queue */
__xdata unsigned char QueueTX[QUEUE_SIZE];
/* variables to access the transmit queue*/
__xdata unsigned char QueueInTX, QueueOutTX;
/* QueueRX is the receive queue */
__xdata unsigned char QueueRX[QUEUE_SIZE];
/* variables to access the receive queue*/
__xdata unsigned char QueueInRX, QueueOutRX;

/* Enum creation for transmitter*/
enum uart_tx_state tx_uart_state;
enum uart_rx_state rx_uart_state;

BOOL uartX_init(enum uart_baud rate, ...)
{
    //Start timer , set CPUCS , initialize the queue
    softuart_init();
    /* If we arrive here we have succesfully initialized UART */
    return TRUE;
}

/**
 * \brief This function sets up TIMER1.
 *  It enables the interrupts and also enables global interrupts
 **/
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

/**
 * \brief Wrapper function for UART initialization
 **/
void softuart_init( void )
{
    timer_init();
    QueueInitRX();
    QueueInitTX();
    SETCPUFREQ(CLK_48M);
    tx_uart_state = IDLE;
    rx_uart_state = IDLE_RX;
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
    if (rx_uart_state == DATA_COMPLETE )
    {
        //Load value
        QueuePutTX(rx_buffer);
        rx_uart_state = IDLE_RX;
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

__bit QueuePutTX(unsigned char data)
{
    //Additional check to make sure there is space in the queue
    if (QueueInTX == (( QueueOutTX - 1 + QUEUE_SIZE) % QUEUE_SIZE))
    {
        return 1; /* Queue Full*/
    }
    QueueTX[QueueInTX] = data;
    QueueInTX = (QueueInTX + 1) % QUEUE_SIZE;
    return 0;
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
void uartX_tx(char c)
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
        if (tx_uart_state == 0x01)
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
                tx_uart_state = 0;
            }
        }
        tx_count = 0x00;
    }
    rx_count = rx_count + 1;
    if (rx_uart_state == IDLE_RX)
    {
        __asm
        anl _OEA, #0xdf;
        mov c, _PA5;
        jc 0001$;
        mov _rx_count, #0x00
        mov _rx_bits_rcvd, #0x00
        mov _rx_uart_state , #0x02
        0001$:
        __endasm;
    }
    if ( (rx_count % 4)  == 0)
    {
        if ((rx_uart_state == START_DETECT) || (rx_uart_state == BUSY_RX))
        {
            rx_uart_state = BUSY_RX;
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
                rx_uart_state = DATA_COMPLETE;
            }
        }
        rx_count = 0x00;
    }
}

BOOL uartX_set_baud(enum uart_baud rate)
{
    return FALSE;
}

enum uart_baud uartX_get_baud()
{
    return BAUD_9600;
}

BOOL uartX_tx_willblock()
{
    return FALSE;
}

char uartX_rx()
{
//This function should never be called
    return 0xFF;
}

BOOL uartX_check_rx_blocking()
{
//The timer based UART does not block
    return FALSE;
}

BYTE uartX_check_receive_buffer()
{
//Read not implemented. Always return a 0.
    return 0x00;
}
