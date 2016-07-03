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






struct mpsse_ep2_buffer {
    /* Pointer to the buffer */
    __xdata unsigned char *DAT;
    /*
     * MPSSE command length .The field below is 2 bytes long and
     * copies the EP2BCL,EP2BCH and is used in all computation.
     */
    unsigned short total_length;
    /* Current BYTE which is being used */
    unsigned short current_index;
};

//#define get_next_byte() ep2_buffer.DAT[++ep2_buffer.current_index]
#define get_current_byte() ep2_buffer.DAT[ep2_buffer.current_index]
#define get_prev_byte() ep2_buffer.DAT[--ep2_buffer.current_index]
#define get_current_length() ep2_buffer.total_length


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
 BYTE bitbang_enabled;
 /*This pointer is set by the function, before filling the IN buffer*/
 unsigned char *readbuffer;
};

/**
 * MPSSE read/write commands.See section 3.6 for more information.
 * This structures controls the operation of the MPSSE engine.
 * Modelled after ftdi_context.
**/
struct mpsse_read_write {
 /*Mode of operations
  * 0x80 - Set Data bits low.
  * 0x82 - Set Data bits high.
  * 0x81 - Read Data bits low.
  * 0x83 - Read Data bits high.
 */
 BYTE command;
 /*The value to write*/
 BYTE value;
 /*The direction to set(OEA pins)*/
 BYTE direction;
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

enum mpsse_clocking_commands
    {
    CLOCK_BYTES_OUT_POS_MSB         = 0x10,
    CLOCK_BYTES_OUT_NEG_MSB         = 0x11,
    CLOCK_BITS_OUT_POS_MSB          = 0x12,
    CLOCK_BITS_OUT_NEG_MSB          = 0x13,
    CLOCK_BYTES_IN_POS_MSB          = 0x20,
    CLOCK_BYTES_IN_NEG_MSB          = 0x24,
    CLOCK_BITS_IN_POS_MSB           = 0x22,
    CLOCK_BITS_IN_NEG_MSB           = 0x26,
    CLOCK_BYTES_IN_OUT_NORMAL_MSB   = 0x31,
    CLOCK_BYTES_IN_OUT_INVERTED_MSB = 0x34,
    CLOCK_BITS_IN_OUT_NORMAL_MSB    = 0x33,
    CLOCK_BITS_IN_OUT_INVERTED_MSB  = 0x36,
    CLOCK_BYTES_OUT_POS_LSB         = 0x18,
    CLOCK_BYTES_OUT_NEG_LSB         = 0x19,
    CLOCK_BITS_OUT_POS_LSB          = 0x1A,
    CLOCK_BITS_OUT_NEG_LSB          = 0x1B,
    CLOCK_BYTES_IN_POS_LSB          = 0x28,
    CLOCK_BYTES_IN_NEG_LSB          = 0x2C,
    CLOCK_BITS_IN_POS_LSB           = 0x2A,
    CLOCK_BITS_IN_NEG_LSB           = 0x2E,
    CLOCK_BYTES_IN_OUT_NORMAL_LSB   = 0x39,
    CLOCK_BYTES_IN_OUT_INVERTED_LSB = 0x3C,
    CLOCK_BITS_IN_OUT_NORMAL_LSB    = 0x3B,
    CLOCK_BITS_IN_OUT_INVERTED_LSB  = 0x3E
    };

/**
 * \brief Called when the FTDI_RESET command is issued.
**/
 void mpsse_reset();

/**
 * \brief Called whenever a SETUP packet is received and the bmRequest is
 * 0x40 or 0xc0(vendor commands).
**/
 void mpsse_handle_control();

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
 void mpsse_handle_bulk();

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
 * \brief Clocks data out depending on length bytes(positive edge).
 * \param The data is present in the endpoint buffer, and offset
 * indicates the byte position of the length field inside the endpoint buffer
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_obyte_data_pos(__bit dir);
/**
 * \brief Clocks data out depending on length bytes(negative edge).
 * \param offset - The data is present in the endpoint buffer, and offset
 * indicates the byte position of the length field inside the endpoint buffer
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_obyte_data_neg(__bit dir);

/**
 * \brief Clocks data bits out depending on length field(positive edge).
 * \param offset The data is present in the endpoint buffer, and offset
 * indicates the byte position of the length field.
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_obits_data_pos(__bit dir);

/**
 * \brief Clocks data bits out depending on length field(negative edge).
 * \param offset The data is present in the endpoint buffer, and offset
 * indicates the byte position of the length field.
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_obits_data_neg(__bit dir);


/**
 * \brief Clocks data in depending on length bytes(positive edge).
 * \param offset Location of length field in EP buffer.
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_ibyte_data_pos(__bit dir);

/**
 * \brief Clocks data in depending on length bytes(negative edge).
 * \param offset Location of length field in EP buffer.
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_ibyte_data_neg(__bit dir);

/**
 * \brief Clocks data bits in depending on length field(positive edge).
 * \param offset offset Location of length field in EP buffer.
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_ibits_data_pos(__bit dir);

/**
 * \brief Clocks data bits out(negative edge) depending on length field(negative edge).
 * \param offset Location of length field in EP buffer.
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_ibits_data_neg(__bit dir);

/**
 * \brief Clocks data bytes in and out depending on length bytes(positive edge).
 * \param offset Location of length field in EP buffer. Normal operation is
 * out on -ve,in on +ve.
 * \param polarity 0 - normal mode, 1 - polarity inverted
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_iobyte_data( __bit polarity,__bit dir);

/**
 * \brief Clocks data bits in and out depending on length bytes(positive edge).
 * \param offset Location of length field in EP buffer. Normal operation is
 * out on -ve,in on +ve.
 * \param polarity 0 - normal mode, 1 - polarity inverted
 * \param dir
 *    \li MSB(0 , MSB clocked out first)
 *    \li LSB(1 , LSB clocked out first)
**/
void clock_iobits_data(__bit polarity,__bit dir);



/**
 * Configures timer 1 for shifting operations.
**/
void mpsse_configure_timer();

/**
 * Send ACK on Endpoint 1.
 * \param type - The ACK to send out.
 *    \li INITIAL  ACK(0 , Data is being shifted out)
 *    \li REPEAT ACK(1 , Data is being shifted in)
**/
void send_endpoint_flush(unsigned char type);

/**
 * \brief Decrements length and also checks for rollover.
 * \param length - The parameter to decrement by.
**/
void decrement_total_byte_count(unsigned char length);

/**
 * \brief Gets the next byte in the buffer. This is a transparent
 * function. It looks at the length and automatically rearms the endpoint
 * when the length has reached 512 bytes.
**/
unsigned char get_next_byte();

/**
 * \brief Inserts data into EP1IN buffer. If count is more than 64,
 * it sends the data out and blocks till an ACK has been received
 * from the host.
**/
void put_ep1in_data();

/**
 * Enum controlling ISR operation.
 *    \li TX(0 , Data is being shifted out)
 *    \li RX(1 , Data is being shifted in)
**/
enum mpsse_isr_mode
{
   TX = 0,
   RX = 1
};

/**
 * Enum controlling flush operation.
 *    \li INITIAL FLUSH(0x32,0x70,0x00 sent out)
 *    \li REPEAT_FLUSH(1 , 0x32,0x60,0x00 sent out)
**/
enum mpsse_flush_mode
{
   INITIAL_ACK = 0,
   REPEAT_ACK = 1
};

/**
 * Enum controlling ISR mode.
**/
enum mpsse_isr_state
{
   IDLE = 0,    //Data from the buffer has been read out.
   BUSY = 1 ,   //Operation is ongoing.
   COMPLETE = 2 //Operation is complete , data now needs to be read out.
};

/**
 * Controls the number of bits to be shifted in or out.
**/
extern unsigned char volatile mpsse_bit_count;

/**
 * The buffer which holds data which is to be shifted out or has been shifted in.
**/
extern unsigned char volatile mpsse_isr_buffer;

/**
 * Allow the struct to be accessed from anyfile that includes this header file.
**/
extern __xdata __at(0xE6B8) volatile struct mpsse_control_request control_request;
extern __xdata __at(0xF000) volatile struct mpsse_read_write read_write;
extern volatile enum  mpsse_isr_state isr_state;
extern volatile enum mpsse_isr_mode isr_mode;
/**
 * Keeps track of how many times the ISR has fired.
**/
extern unsigned char isr_enter;

/**
 * Keeps track of how many bytes are read and inserted into the EP1IN buffer.
**/
extern unsigned char ep1in_buffer_length;


//DELETE:
void putchar(char c);
void uart_tx_unsigned(unsigned char c);


#endif // MPSSE_UTILS_H
