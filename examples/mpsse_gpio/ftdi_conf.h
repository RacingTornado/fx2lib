/** \file ftdi_conf.h
 * Definitions for FTDI devices
 **/
#ifndef FTDI_CONF_H
#define FTDI_CONFH
#include "fx2types.h"
#include <delay.h>
#include <fx2regs.h>
// These defines came from the Linux source code:
// drivers/usb/serial/ftdi_sio.h
// That file also does a reasonable job of documenting the protocol,
// particularly the vendor requests I don't implement that do things
// like setting the baud rate.
#define FTDI_RS0_CTS    (1 << 4)
#define FTDI_RS0_DSR    (1 << 5)
#define FTDI_RS0_RI     (1 << 6)
#define FTDI_RS0_RLSD   (1 << 7)
#define FTDI_RS_DR  1
#define FTDI_RS_OE (1<<1)
#define FTDI_RS_PE (1<<2)
#define FTDI_RS_FE (1<<3)
#define FTDI_RS_BI (1<<4)
#define FTDI_RS_THRE (1<<5)
#define FTDI_RS_TEMT (1<<6)
#define FTDI_RS_FIFO  (1<<7)
//Used for delay definition
#define SYNCDELAY SYNCDELAY4
/**
 * \brief Configures endpoint 1 
**/
void configure_endpoints(void);
/**
 * \brief Send data present in EP1INBUF out 
**/
void process_send_data(void);
/**
 * \brief Inserts data into endpoint buffer 
**/
void putchar(char c);
/**
 * \brief Receives data from endpoint 1, and calls putchar_usb
**/
void process_recv_data(void);
#endif
