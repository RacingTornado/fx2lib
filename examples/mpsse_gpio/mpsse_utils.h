/** \file mpsse_utils.h
 * This file is for defining layered structure which can be used
 * for handling MPSSE commands. A lot of the #define statements have
 * been taken from ftdi.h in libftdi
**/
#ifndef MPSSE_UTILS_H
#define MPSSE_UTILS_H

#include <fx2types.h>

/* Shifting commands IN MPSSE Mode. See section 3.2
Bit 0 : -ve CLK on write
Bit 1 : bit mode = 1 else byte mode
Bit 2 : -ve CLK on read
Bit 3 : LSB first = 1 else MSB first
Bit 4 : Do write TDI
Bit 5 : Do read  TDO
Bit 6 : Do writeTMS
Bit 7 : 0 
*/
#define MPSSE_WRITE_NEG 0x01   /* Write TDI/DO on negative TCK/SK edge,bit 0 has been set */
#define MPSSE_BITMODE   0x02   /* Write bits, not bytes, set bit 1 */
#define MPSSE_READ_NEG  0x04   /* Sample TDO/DI on negative TCK/SK edge , set bit 2*/
#define MPSSE_LSB       0x08   /* LSB first , set bit 3 */
#define MPSSE_DO_WRITE  0x10   /* Write TDI/DO , set bit 4*/
#define MPSSE_DO_READ   0x20   /* Read TDO/DI */
#define MPSSE_WRITE_TMS 0x40   /* Write TMS/CS */

/* FTDI MPSSE commands, these are sent from the host as bulk commands*/
#define SET_BITS_LOW   0x80 /*These set the ADBUS[7-0] */
/*BYTE DATA*/
/*BYTE Direction*/
#define SET_BITS_HIGH   0x82 /*These set the ACBUS[7-0] */
/*BYTE DATA*/
/*BYTE Direction*/
#define GET_BITS_LOW   0x81
#define GET_BITS_HIGH  0x83
/*Used for device teseting, internally connects the 2 pins*/
#define LOOPBACK_START 0x84
#define LOOPBACK_END   0x85
#define TCK_DIVISOR    0x86
/* H Type specific commands */
#define DIS_DIV_5       0x8a
#define EN_DIV_5        0x8b
#define EN_3_PHASE      0x8c
#define DIS_3_PHASE     0x8d
#define CLK_BITS        0x8e
#define CLK_BYTES       0x8f
#define CLK_WAIT_HIGH   0x94
#define CLK_WAIT_LOW    0x95
#define EN_ADAPTIVE     0x96
#define DIS_ADAPTIVE    0x97
#define CLK_BYTES_OR_HIGH 0x9c
#define CLK_BYTES_OR_LOW  0x9d
/*FT232H specific commands */
#define DRIVE_OPEN_COLLECTOR 0x9e
/* Value Low */
/* Value HIGH */ /*rate is 12000000/((1+value)*2) */
#define DIV_VALUE(rate) (rate > 6000000)?0:((6000000/rate -1) > 0xffff)? 0xffff: (6000000/rate -1)
/* Commands in MPSSE and Host Emulation Mode */
#define SEND_IMMEDIATE 0x87
#define WAIT_ON_HIGH   0x88
#define WAIT_ON_LOW    0x89
/* Commands in Host Emulation Mode */
#define READ_SHORT     0x90
/* Address_Low */
#define READ_EXTENDED  0x91
/* Address High */
/* Address Low  */
#define WRITE_SHORT    0x92
/* Address_Low */
#define WRITE_EXTENDED 0x93
/* Address High */
/* Address Low  */
/* Definitions for flow control */
#define SIO_RESET          0 /* Reset the port */
#define SIO_MODEM_CTRL     1 /* Set the modem control register */
#define SIO_SET_FLOW_CTRL  2 /* Set flow control register */
#define SIO_SET_BAUD_RATE  3 /* Set baud rate */
#define SIO_SET_DATA       4 /* Set the data characteristics of the port */

/* Requests , these arrive on a control endpoint*/
#define SIO_RESET_REQUEST             SIO_RESET
#define SIO_SET_BAUDRATE_REQUEST      SIO_SET_BAUD_RATE
#define SIO_SET_DATA_REQUEST          SIO_SET_DATA
#define SIO_SET_FLOW_CTRL_REQUEST     SIO_SET_FLOW_CTRL
#define SIO_SET_MODEM_CTRL_REQUEST    SIO_MODEM_CTRL
#define SIO_POLL_MODEM_STATUS_REQUEST 0x05
#define SIO_SET_EVENT_CHAR_REQUEST    0x06
#define SIO_SET_ERROR_CHAR_REQUEST    0x07
#define SIO_SET_LATENCY_TIMER_REQUEST 0x09
#define SIO_GET_LATENCY_TIMER_REQUEST 0x0A
#define SIO_SET_BITMODE_REQUEST       0x0B
#define SIO_READ_PINS_REQUEST         0x0C
#define SIO_READ_EEPROM_REQUEST       0x90
#define SIO_WRITE_EEPROM_REQUEST      0x91
#define SIO_ERASE_EEPROM_REQUEST      0x92
/* Reset commands , control endpoint*/ 
#define SIO_RESET_SIO 0
#define SIO_RESET_PURGE_RX 1
#define SIO_RESET_PURGE_TX 2

BYTE handle_get_configuration();
BOOL handle_set_configuration(BYTE cfg);
BOOL handle_get_interface(BYTE ifc, BYTE* alt_ifc);
BOOL handle_set_interface(BYTE ifc, BYTE alt_ifc);
BOOL handle_get_descriptor();

/* Prints out the control endpoint request.*/ 
#define PRINT_REQUEST(str) \
	printf( \
		str " bControl:%d bRequest:%s bLength: %d\n", \
		uvc_ctrl_request.wValue.bControl, \
		uvc_control_request_str(uvc_ctrl_request.bRequest), \
		uvc_ctrl_request.wLength);
/**
 * MPSSE control packet. All control packets are sent on endpoint 0.
 * This structures handles data from endpoint 0.(Control_Endpoint)
**/
struct mpsse_control_request {
 	/* Request Type, Direction, and Recipient */
	BYTE bmRequestType;
	/* The actual request.TRM See section 2.3. Decides the operation */
	BYTE bRequest;
	/* The values in this struct provide data for operation specified in bRequest */
	struct {
		BYTE bZero;
		BYTE bControl;
	} wValue;
	/* The values in this struct define the interface
	 *  \li INTERFACE_ANY
	 *  \li INTERFACE_A
	 *  \li INTERFACE_B(not supported)
	 *  \li INTERFACE_C(not supported)
	 *  \li INTERFACE_D(not supported)
	 */
	struct {
		BYTE bInterface;
		BYTE bUnitId;
	} wIndex;
	/* The number of bytes to follow the SETUPDAT request */
	WORD wLength;
};

/**
 * MPSSE context. 
 * This structures controls the operation of the MPSSE engine.
 * Modelled after ftdi_context.
**/
struct mpsse_context {
 /*Mode of operations */
 enum ftdi_mpsse_mode mode;
 /*Interface on which chio should operate */
 enum ftdi_interface  interface;
 /*Unused variable */
 BYTE baud_rate;
 /*Set to 1 if bitbang is enabled*/
 __bit bitbang_enabled;
 /*This pointer is set by the function, before filling the IN buffer*/
 unsigned char *readbuffer;
};

/**
 * Supported modes by the FT2232H chip.
**/
enum ftdi_mpsse_mode
    {
        BITMODE_RESET  = 0x00,    
        BITMODE_BITBANG= 0x01,    
        BITMODE_MPSSE  = 0x02,    
        BITMODE_SYNCBB = 0x04,    
        BITMODE_MCU    = 0x08,    
        /* CPU-style fifo mode gets set via EEPROM.*/
        BITMODE_OPTO   = 0x10,    
        BITMODE_CBUS   = 0x20,    
        BITMODE_SYNCFF = 0x40,    
        BITMODE_FT1284 = 0x80,    
    };

/**
 * Supported interfaces. 
**/
enum ftdi_interface
    {
        INTERFACE_ANY = 0,
        INTERFACE_A   = 1, //0x81,0x02 endpoints
        INTERFACE_B   = 2, //0x83,0x04 endpoints
        INTERFACE_C   = 3, //0x85,0x06 endpoints
        INTERFACE_D   = 4  //0x87,0x08 endpoints
    };

/**
 * \brief Called when the FTDI_RESET command is issued.
**/
 void mpsse_reset();

/**
 * \brief Called whenever a SETUP packet is received and the bmRequest is 
 * 0x40 or 0xc0(vendor commands).
**/
 void mpsse_parse_control();

/**
 * \brief Called whenever a SETUP packet is received and the bmRequest is 
 * 0x40 or 0xc0(vendor commands).
**/
 void set_baud_rate(BYTE rate);

/**
 * \brief Sets the latency timer(currently unimplemented).
**/
 void set_latency_timer(BYTE latency);

/**
 * \brief Sets the bitmode to mode.
**/
 void set_bitmode(BYTE mode);

/**
 * \brief Setting the pins occurs via URB bulk request.
**/
 void parse_bulk_control();

/**
 * \brief Setting the pins occurs via URB bulk request.
**/
 void set_pins_state(BYTE value, BYTE direction);

/**
 * \brief Get the pin state indicated by pin.
**/
 BYTE get_pins_state(BYTE pins);

/**
 * \brief Clear the RX buffer.
**/
void purge_rx_buffer();

/**
 * \brief Clear the TX buffer.
**/
void purge_tx_buffer();

/**
 * \brief Configures the endpoints according to the FT2232H.
**/
void configure_endpoints();

/**
 * Allow the struct to be accessed from anyfile that includes this header file.
**/
extern struct mpsse_control_request mpsse_control;

#endif // MPSSE_UTILS_H
