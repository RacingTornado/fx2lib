/** \file softuart.h
 * Utilities for reading and writing to UART devices.
 * Defines top level functions which the printf and scanf access. 
 * The implementation of these functions vary depending on whether 
 * fast_uart is being used or timer based UART is being used. This 
 * is configurable depending on the way uart0_init() is called
 **/

#ifndef FX2_UART
#define FX2_UART

#include "fx2types.h"

/**
 * \brief initalizes UART.
 * This function uart0_init accepts the baud_rate and the mask. The mask 
 * indicates where the UART_TX pin must be attached. The rx_mask performs a 
 * similar function. The function will then use a case statement to identify 
 * the correct location and then do a .equ TX_PIN to define the transmit pin. 
 *   Possible Baud rates:(fast_uart)
 *    \li 2400
 *    \li 4800
 *    \li 9600
 *    \li 19200
 *    \li 28800
 *    \li 38400
 *    \li 57600
 *    \li 115200
 *    This function sets the CPUCS to 48Mhz . It then uses assembly to 
 *    delay the pin timing to get the required baud rate with the necessary
 *    accuracy or initializes and starts a timer depending on the situation. 
 *    This function will map to a timer based delay function 
 *    in case of a the timer based uart. The only restriction is that this 
 *    function will not be able to have a baud rate of a value greater 
 *    than 9600 , if an ISR is used to shift the data out. This is because 
 *    the UART RX logic should run much faster than the TX logic to
 *    detect the start bit. A decision to have the same UART TX/RX rates 
 *    was chosen. Thus the allowable baud rates for this function is
 *   Possible Baud rates:(timer based uart)
 *    \li 2400
 *    \li 4800
 *    \li 9600
 * \param rate enum for baud_rate
 * \param type enum for selecting what type of UART is actually used
 * \param tx_pin The pin to which the UART_TX routine must be attached
 * \param rx_pin The pin to which the UART_RX routine must be attached
 **/
void uart0_init (enum uart_baud rate, enum uart_type type, enum pins_fx2 tx_pin, enum pins_fx2 rx_pin) __critical;

/**
 * \brief transmits data through UART.
 * The uart0_transmit maps to different functions depending on the underlying
 * mechanism which is used to transmit the byte. In the case of bitbanging UART
 * without timers, it becomes blocking.  If timer based UART is used, then 
 * it is non blocking. It puts the data into the queue and returns without 
 * doing anything else. The ISR then handles the shifting of the data out.  
 * Be careful regarding queue overflows when using non-blocking UART,  
 * that is make sure there is enough time between calls to printf.
 * \param c character to be written to UART
**/
void uart0_transmit (char c);

/**
 * \brief receives data through UART.
 * This function uart0_receive is basically empty for fast_uart. However, in case of  
 * timer based UART. It reads data from the queue, and returns a single character which 
 * can then be used by the calling program.
 *
**/
char uart0_receive ();

/**
 * enum used for easy access for baud rate selection.
 *
**/
enum uart_baud { U_2400, U_4800, U_9600, U_19200, U_38400, U_57600, U_115200 };
/**
 * Defines the type of UART which is used to send out the bits. If FAST_UART is used , high baud
 * rates can be achieved. However, only transmit functionality will be supported, and the function
 * call will block. TIMER_UART on the other hand uses a Queue(based on buffers) and does not block.
 * It allows both transmit and receive functionality , however the baud rates supported are much slower.
 *
**/
enum uart_type { FAST_UART , TIMER_UART };
/**
 * enum used for easy access to fx2 pins
 *
**/
enum pins_fx2 { PA_0,PA_1,PA_2,PA_3,PA_4,PA_5,PA_6,PA_7,
		PB_0,PB_1,PB_2,PB_3,PB_4,PB_5,PB_6,PB_7,
		PD_0,PD_1,PD_2,PD_3,PD_4,PD_5,PD_6,PD_7
 };

#endif
