#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"
#include "mpsse_utils.h"
#include "mpsse_functions.h"

__xdata __at(0xE6B8) volatile struct mpsse_control_request control_request;
__xdata __at(0xF000) volatile struct mpsse_read_write read_write;
__xdata enum mpsse_clocking_commands clocking_commands;
__xdata struct mpsse_ep2_buffer ep2_buffer;
/*******************************************************
These variables have already been declared in mpsse_util.h
********************************************************/
volatile enum  mpsse_isr_state isr_state;
volatile enum  mpsse_isr_mode isr_mode;
unsigned char volatile mpsse_isr_buffer;
unsigned char ep1in_buffer_length = 0;


/*********************************************************************
Temporary variables for use in this file. Not declared in mpsse_util.h
**********************************************************************/
/**
 * Used to keep track of number of bytes to clock out or in.
**/
unsigned short mpsse_byte_clock_length;
/**
 * Used to keep track of number of bits to clock out or in.
**/
unsigned char mpsse_bits_clock_length;

/**
 * Used to keep track of data being sent in or out.
**/
unsigned char data_epbuf;

//DELETE
unsigned char delete_packets_read;

void uart_tx(char c);

void mpsse_reset()
{
    //Initialize to IDLE state.
    isr_state = IDLE;
    //Number of bits which should be shifted in or out.
    mpsse_bit_count = 0;
    isr_mode  = TX;
    ep1in_buffer_length = 0;
}

void mpsse_handle_control()
{
    printf("Control %02x\r\n",SETUPDAT[1]);
    switch (SETUPDAT[1])
    {
    case SIO_RESET_REQUEST:
    {
        printf("Resetting %02x\r\n",SETUPDAT[1]);
        EP0CS |= 0x80;
    }
    break;
    case SIO_SET_BAUD_RATE:
    {
        EP0CS |= 0x80;

    }
    break;
    case SIO_SET_LATENCY_TIMER_REQUEST:
    {
        EP0CS |= 0x80;
    }
    break;
    case SIO_SET_BITMODE_REQUEST:
    {
        EP0CS |= 0x80;
    }
    break;
    case SIO_READ_PINS_REQUEST:
    {
        /* The FT2232 docs and the libftdi just do a read. I am not sure
         * which pin to return. Some examples return the CBUS. So , we will
         * return port A
         */
        OEA = 0x00;
        EP0BUF[0] = IOA;
        EP0BCL = 0x01;
        EP0BCH = 0x00;
        EP0CS |= 0x80;
    }
    break;
    case SIO_SET_EVENT_CHAR_REQUEST:
    {
        EP0CS |= 0x80;
    }
    break;
    default:
        break;
    }

}


void configure_endpoints()
{
    /*
      Interface A is the only supported mode in FX2 currently
      We need to configure endpoint 1 as IN, and endpoint 2
      as OUT endpoint.
      case INTERFACE_A:
               ftdi->interface = 0;
               ftdi->index     = INTERFACE_A;
               ftdi->in_ep     = 0x02;
               ftdi->out_ep    = 0x81;
    */
    EP1INCFG = 0xa0;
    SYNCDELAY;
    /*Out endpoint, double buffered, bulk endpoint*/
    EP2CFG = 0xa2;
    FIFORESET = 0x80;    // Reset the FIFO
    SYNCDELAY;
    FIFORESET = 0x88;
    SYNCDELAY;
    FIFORESET = 0x00;

}

void putchar(char c)
{
    uart_tx(c);
}

void mpsse_handle_bulk()
{
    /* Pin mapping docs details how the ports on FX2 are mapped to
       those on FT2232H */
    //Points to the endpoint 2 FIFO buffer
    ep2_buffer.DAT = EP2FIFOBUF;
    //Store the total length of bytes received
    ep2_buffer.total_length = EP2BCL | (EP2BCH << 8);
    ep2_buffer.current_index = 65535;
    printf("Length is %02d\r\n",ep2_buffer.total_length);
    while(ep2_buffer.total_length!=0)
    {
        get_next_byte();
        printf("Current byte is %02x and length is %02d with total length %02dr\n",get_current_byte(),ep2_buffer.current_index,ep2_buffer.total_length);
        switch(get_current_byte())
        {
        case CLOCK_BYTES_OUT_POS_MSB:
            func_CLOCK_BYTES_OUT_POS_MSB();
            break;
        case CLOCK_BYTES_OUT_NEG_MSB:
            func_CLOCK_BYTES_OUT_NEG_MSB();
            break;
        case CLOCK_BITS_OUT_POS_MSB:
            func_CLOCK_BITS_OUT_POS_MSB();
            break;
        case CLOCK_BITS_OUT_NEG_MSB:
            func_CLOCK_BITS_OUT_NEG_MSB();
            break;
        case CLOCK_BYTES_IN_POS_MSB:
            func_CLOCK_BYTES_IN_POS_MSB();
            break;
        case CLOCK_BYTES_IN_NEG_MSB:
            func_CLOCK_BYTES_IN_NEG_MSB();
            break;
        case CLOCK_BITS_IN_POS_MSB:
            func_CLOCK_BITS_IN_POS_MSB();
            break;
        case CLOCK_BITS_IN_NEG_MSB:
            func_CLOCK_BITS_IN_NEG_MSB();
            break;
        case CLOCK_BYTES_IN_OUT_NORMAL_MSB:
            func_CLOCK_BYTES_IN_OUT_NORMAL_MSB();
            break;
        case CLOCK_BYTES_IN_OUT_INVERTED_MSB:
            func_CLOCK_BYTES_IN_OUT_INVERTED_MSB();
            break;
        case CLOCK_BITS_IN_OUT_NORMAL_MSB:
            func_CLOCK_BITS_IN_OUT_NORMAL_MSB();
            break;
        case CLOCK_BITS_IN_OUT_INVERTED_MSB:
            func_CLOCK_BITS_IN_OUT_INVERTED_MSB();
            break;
        case CLOCK_BYTES_OUT_POS_LSB:
            func_CLOCK_BYTES_OUT_POS_LSB();
            break;
        case CLOCK_BYTES_OUT_NEG_LSB:
            func_CLOCK_BYTES_OUT_NEG_LSB();
            break;
        case CLOCK_BITS_OUT_POS_LSB:
            func_CLOCK_BITS_OUT_POS_LSB();
            break;
        case CLOCK_BITS_OUT_NEG_LSB:
            func_CLOCK_BITS_OUT_NEG_LSB();
            break;
        case CLOCK_BYTES_IN_POS_LSB:
            func_CLOCK_BYTES_IN_POS_LSB();
            break;
        case CLOCK_BYTES_IN_NEG_LSB:
            func_CLOCK_BYTES_IN_NEG_LSB();
            break;
        case CLOCK_BITS_IN_POS_LSB:
            func_CLOCK_BITS_IN_POS_LSB();
            break;
        case CLOCK_BITS_IN_NEG_LSB:
            func_CLOCK_BITS_IN_NEG_LSB();
            break;
        case CLOCK_BYTES_IN_OUT_NORMAL_LSB:
            func_CLOCK_BYTES_IN_OUT_NORMAL_LSB();
            break;
        case CLOCK_BYTES_IN_OUT_INVERTED_LSB:
            func_CLOCK_BYTES_IN_OUT_INVERTED_LSB();
            break;
        case CLOCK_BITS_IN_OUT_NORMAL_LSB:
            func_CLOCK_BITS_IN_OUT_NORMAL_LSB();
            break;
        case CLOCK_BITS_IN_OUT_INVERTED_LSB:
            func_CLOCK_BITS_IN_OUT_INVERTED_LSB();
            break;
        case CLOCK_DATA_TMS_POS:
            func_CLOCK_DATA_TMS_POS();
            break;
        case CLOCK_DATA_TMS_NEG:
            func_CLOCK_DATA_TMS_NEG();
            break;
        case TMS_READ_POS_POS:
            func_TMS_READ_POS_POS();
            break;
        case TMS_READ_NEG_POS:
            func_TMS_READ_NEG_POS();
            break;
        case TMS_READ_POS_NEG:
            func_TMS_READ_POS_NEG();
            break;
        case TMS_READ_NEG_NEG:
            func_TMS_READ_NEG_NEG();
            break;
        case SET_BITS_LOW:
            func_SET_BITS_LOW();
            break;
        case SET_BITS_HIGH:
            func_SET_BITS_HIGH();
            break;
        case GET_BITS_LOW:
            func_GET_BITS_LOW();
            break;
        case GET_BITS_HIGH:
            func_GET_BITS_HIGH();
            break;
        case LOOPBACK_START:
            func_LOOPBACK_START();
            break;
        case LOOPBACK_END:
            func_LOOPBACK_END();
            break;
        case TCK_DIVISOR:
            func_TCK_DIVISOR();
            break;
        case SEND_IMMEDIATE:
            func_SEND_IMMEDIATE();
            break;
        case WAIT_ON_HIGH:
            func_WAIT_ON_HIGH();
            break;
        case WAIT_ON_LOW:
            func_WAIT_ON_LOW();
            break;
        case DIS_DIV_5:
            func_DIS_DIV_5();
            break;
        case EN_DIV_5:
            func_EN_DIV_5();
            break;
        case EN_3_PHASE:
            func_EN_3_PHASE();
            break;
        case DIS_3_PHASE:
            func_DIS_3_PHASE();
            break;
        case CLK_BITS:
            func_CLK_BITS();
            break;
        case CLK_BYTES:
            func_CLK_BYTES();
            break;
        case CLK_WAIT_HIGH:
            func_CLK_WAIT_HIGH();
            break;
        case CLK_WAIT_LOW:
            func_CLK_WAIT_LOW();
            break;
        case EN_ADAPTIVE:
            func_EN_ADAPTIVE();
            break;
        case DIS_ADAPTIVE:
            func_DIS_ADAPTIVE();
            break;
        case CLK_BYTES_OR_HIGH:
            func_CLK_BYTES_OR_HIGH();
            break;
        case CLK_BYTES_OR_LOW:
            func_CLK_BYTES_OR_LOW();
            break;
        default:
            //decrement_total_byte_count(1);
            printf("Command has not been implemented %02x\r\n",get_current_byte());
            break;
        }
    }
}

void mpsse_configure_timer()
{
    TMOD = 0x20;
    SYNCDELAY;
    TR1 = 0;
    SYNCDELAY;
    TH1 = 0xc3;
    SYNCDELAY;
    TL1 = 0x23;
    SYNCDELAY;
    TR1 = 1;
    SYNCDELAY;
}

void clock_obyte_data_pos(__bit dir)
{


    /* The command has been read. The next 2 bytes gives us the
     * the number of bytes we need to clock out .
     */
    printf("clock_obyte_data_pos\r\n");
    mpsse_byte_clock_length = (get_next_byte() | (get_next_byte()<<8)) + 1;
    printf("Length is %d",mpsse_byte_clock_length);
    //decrement_total_byte_count(3);
    //ep2_buffer.total_length = ep2_buffer.total_length - (mpsse_byte_clock_length);
    while(mpsse_byte_clock_length!=0)
    {
        if(isr_state != BUSY)
        {

            //mpsse_isr_buffer = get_next_byte();
            printf("Clocking out %02x\r\n",mpsse_isr_buffer);
            //mpsse_bit_count  = 0x09;
            //isr_state  = BUSY;
            //isr_mode   = TX;
            //while(isr_state  == BUSY);
            shift_byte_out_JTAG();
            mpsse_byte_clock_length = mpsse_byte_clock_length - 1;
        }

    }
}


void clock_obits_data_pos(__bit dir)
{
    /* The command has been read. The next byte gives use the number of bits
     * to clock out.
     */
    printf("clock_obits_data_pos %02d\r\n",get_current_length());
    mpsse_bits_clock_length = (get_next_byte()) + 1;
    //decrement_total_byte_count(3);
    if(isr_state == IDLE || isr_state == COMPLETE)
    {

        mpsse_isr_buffer = get_next_byte();
        printf("Clocking out %02x\r\n",mpsse_isr_buffer);
        mpsse_bit_count  = mpsse_bits_clock_length + 1;
        isr_state  = BUSY;
        isr_mode   = TX;
        while(isr_state  == BUSY);

    }
}

void clock_obyte_data_neg(__bit dir)
{
    printf("Function currently unimplemented");
    //DELETE THIS
    clock_obyte_data_pos(0);
}

void clock_obits_data_neg(__bit dir)
{
    printf("Function currently unimplemented");
    //DELETE THIS
    clock_obits_data_pos(0);

}

void clock_ibyte_data_pos(__bit dir)
{
    /* The command has been read. The next 2 bytes gives us the
     * the number of bytes we need to read .
     */
    printf("clock_ibyte_data_pos %02x\r\n",get_current_byte());
    mpsse_byte_clock_length = (get_next_byte() | (get_next_byte()<<8)) + 1;
    //decrement_total_byte_count(2);
    //Initialize to 0 once we enter.
    while(mpsse_byte_clock_length!=0)
    {
        if(isr_state == IDLE || isr_state == COMPLETE)
        {

            mpsse_isr_buffer = 0x00;
            mpsse_bit_count  = 0x09;
            mpsse_byte_clock_length = mpsse_byte_clock_length - 1;
            isr_state  = BUSY;
            isr_mode   = RX;
            delete_packets_read++;
            while(isr_state  == BUSY);
            printf("Packets read is %02d\r\n",delete_packets_read);
            put_ep1in_data();
        }

    }
}

void clock_ibits_data_pos(__bit dir)
{
    /* The command has been read.Read a single byte to get
     * bit length.
     */
    printf("clock_ibits_data_pos %02x\r\n",get_current_byte());
    mpsse_bits_clock_length = (get_next_byte()) + 1;
    //decrement_total_byte_count(2);
    printf("Length %02x\r\n",get_current_length());
    if(isr_state == IDLE || isr_state == COMPLETE)
    {
        mpsse_isr_buffer = 0x00;
        mpsse_bit_count  = mpsse_bits_clock_length+1;
        isr_mode   = RX;
        isr_state  = BUSY;
        while(isr_state  == BUSY);


    }
}

void clock_ibyte_data_neg(__bit dir)
{
    printf("Function currently unimplemented");
    //DELETE THIS
    clock_ibyte_data_pos(dir);
}

void clock_ibits_data_neg(__bit dir)
{
    printf("Function currently unimplemented");
    //DELETE THIS
    clock_ibits_data_pos(dir);
}


void clock_iobyte_data( __bit polarity,__bit dir)
{
    printf("NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO -clock_iobyte_data ");
}

void clock_iobits_data(__bit polarity,__bit dir)
{
    printf("NOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO -clock_iobits_data ");
}

void send_endpoint_flush(unsigned char type)
{
    //decrement_total_byte_count(1);
    switch(type)
    {
    case INITIAL_ACK:
        EP1INBUF[0] = 0x32;
        EP1INBUF[1] = 0x70;
        EP1INBUF[2] = 0x01;
        EP1INBC = 0x03;
        break;
    case REPEAT_ACK:
        EP1INBUF[0] = 0x32;
        EP1INBUF[1] = 0x60;
        EP1INBUF[2] = 0x00;
        EP1INBC = 0x03;
        break;
    default :
        //printf("Endpoint flush not implemented\r\n");
    }
    while((EP1INCS&0x02) == 0x02);
    ep1in_buffer_length = 0;
    printf("Flushing data\r\n");
}

void decrement_total_byte_count(unsigned char length)
{
    if( (ep2_buffer.total_length - length) > ep2_buffer.total_length)
    {
        printf("I cant decrement by %02x",length);
    }
    else
    {
        ep2_buffer.total_length = ep2_buffer.total_length - length;
    }
}


unsigned char get_next_byte()
{
    unsigned short counter;
    if(ep2_buffer.current_index == 511 )
    {
        //Rearm the endpoint
        EP2BCL = 0x80; // write once
        //Decrement the isr counter
        SYNCDELAY;
        isr_enter -- ;
        ep2_buffer.total_length = EP2BCL | (EP2BCH << 8);

        printf("Got data in get_next_byte() %02x, length is %04x %02x, buffer len %d",EP2FIFOBUF[6], EP2BCL | (EP2BCH <<8),EP1INCS,ep2_buffer.total_length);
        printf("[ ");
        for(counter =0 ; counter < ep2_buffer.total_length; counter++)
        {
                printf("%02x-%02d, ",EP2FIFOBUF[counter],counter);
        }
        printf("]\r\n");
        ep2_buffer.current_index = 65535;
        decrement_total_byte_count(1);
        delete_total_count = delete_total_count +ep2_buffer.total_length;
        printf("TOTAL length %lu",delete_total_count);
        return ep2_buffer.DAT[++ep2_buffer.current_index];
    }
    else
    {
        decrement_total_byte_count(1);
        return ep2_buffer.DAT[++ep2_buffer.current_index];
    }
}

void put_ep1in_data()
{
    printf("Data length is %02x\r\n",ep1in_buffer_length);
    if(ep1in_buffer_length == 0)
    {
        EP1INBUF[ep1in_buffer_length] = 0x32;
        ep1in_buffer_length++;
        EP1INBUF[ep1in_buffer_length] = 0x00;
        ep1in_buffer_length++;
        EP1INBUF[ep1in_buffer_length] = mpsse_isr_buffer;
        ep1in_buffer_length++;
    }
    else if(ep1in_buffer_length == 40)
    {
        EP1INBC = ep1in_buffer_length;
        //Wait till data has been sent out
        while((EP1INCS&0x02) == 0x02);
        ep1in_buffer_length = 0;
        printf("put_ep1in_data\r\n");
        EP1INBUF[ep1in_buffer_length] = 0x32;
        ep1in_buffer_length++;
        EP1INBUF[ep1in_buffer_length] = 0x00;
        ep1in_buffer_length++;
        EP1INBUF[ep1in_buffer_length] = mpsse_isr_buffer;
        ep1in_buffer_length++;
    }
    else
    {
        EP1INBUF[ep1in_buffer_length] = mpsse_isr_buffer;
        ep1in_buffer_length++;
    }



}

void flush_ep1in_data()
{
    if(ep1in_buffer_length!=0)
    {

        EP1INBC = ep1in_buffer_length;
        //Wait till data has been sent out
        while((EP1INCS&0x02) == 0x02);
        ep1in_buffer_length = 0;
        printf("Flushing data\r\n");
    }
}



















//-----------------------------------------------------------------------------

/* JTAG TCK, AS/PS DCLK */

__sbit __at 0x80          TCK; /* Port A.0 */


/* JTAG TDI, AS ASDI, PS DATA0 */

__sbit __at 0x82         TDO; /* Port A.2 */


/* JTAG TMS, AS/PS nCONFIG */

__sbit __at 0x81          TDI; /* Port A.1 */


/* JTAG TDO, AS/PS CONF_DONE */

__sbit __at 0x83          TMS; /* Port A.3 */




/*
;; For ShiftInOut, the timing is a little more
;; critical because we have to read _TDO/shift/set _TDI
;; when _TCK is low. But 20% duty cycle at 48/4/5 MHz
;; is just like 50% at 6 Mhz, and that's still acceptable
*/

void read_write_bits_JTAG()
{
    printf("Read TDO and write TDI bits %02x\r\n",get_current_byte());
    mpsse_bits_clock_length = get_next_byte() + 1;
    data_epbuf = get_next_byte();
  __asm
        MOV  A,_data_epbuf
        mov r0,_mpsse_bits_clock_length
        0001$:
        MOV  C,_TDO
        RRC  A
        MOV  _TDI,C
        SETB _TCK
        CLR  _TCK
        djnz r0, 0001$
        MOV  _mpsse_isr_buffer,A
  __endasm;
  /* Insert the data back into the buffer */
  put_ep1in_data();
}





/* Change the state of TAP controller only. Don't read anything from TDO/TDI */
void clock_bits_tms()
{

    mpsse_bits_clock_length = (get_next_byte()) + 1;
    data_epbuf = get_next_byte();
    printf("4b clocking %02x bits with value %02x\r\n",mpsse_bits_clock_length,data_epbuf);
     __asm
        MOV  A,_data_epbuf                  ;Move the data into the accumulator
        MOV r0,_mpsse_bits_clock_length
        0001$:RRC  A
        CLR  _TCK
        MOV  _TMS,C
        SETB _TCK
        djnz r0,0001$
        nop
        CLR  _TCK
  __endasm;


}

/* Read TDO, write TMS */
unsigned char read_bits_write_TMS_JTAG()
{
    /* Write and read x bits from JTAG */
    printf("Read TDI, clock out TMS %02x\r\n",get_current_byte());
    mpsse_bits_clock_length = get_next_byte();
    data_epbuf = get_next_byte();
  __asm
        MOV  A,_data_epbuf
        mov r0,_mpsse_bits_clock_length
        0001$:
        MOV  C,_TDO
        RRC  A
        MOV  _TMS,C
        SETB _TCK
        CLR  _TCK
        djnz r0, 0001$
        MOV  _mpsse_isr_buffer,A
  __endasm;
  /* Insert the data back into the buffer */
  put_ep1in_data();

}

/* Read and writes bytes for JTAG*/
void read_write_bytes_JTAG()
{
    printf("Reading and writing bytes from JTAG %02x\r\n",get_current_byte());
    mpsse_byte_clock_length = (get_next_byte() | (get_next_byte()<<8)) + 1;
    while(mpsse_byte_clock_length!=0)
    {
            shift_bytes_JTAG();
            mpsse_byte_clock_length = mpsse_byte_clock_length - 1;
            put_ep1in_data();
    }
}

/* Shifts exactly 1 byte out, and reads one byte in */
void shift_bytes_JTAG()
{
    printf("Shifting data in and out for JTAG %02x\r\n",get_current_byte());
    data_epbuf = get_next_byte();
    EA = 0;
  __asm
        MOV  A,_data_epbuf
        mov r0,#0x08
        0001$:
        MOV  C,_TDO
        RRC  A
        MOV  _TDI,C
        nop
        nop
        SETB _TCK
        nop
        nop
        CLR _TCK
        djnz r0, 0001$                  ;Stop if 8 bits have been shifted, we have to reload buffers.
        mov _mpsse_isr_buffer,a         ;Move the data into the buffer to be read
  __endasm;
  EA = 1;
}

void shift_byte_out_JTAG()
{
    printf("Shifting data in and out for JTAG %02x\r\n",get_current_byte());
    data_epbuf = get_next_byte();
    EA = 0;
//  __asm
//        MOV  A,_data_epbuf
//        mov r0,#0x08
//        0001$:
//        RRC  A
//        MOV  _TDI,C
//        nop
//        nop
//        SETB _TCK
//        nop
//        nop
//        CLR  _TCK
//        djnz r0, 0001$                  ;Stop if 8 bits have been shifted, we have to reload buffers.
//  __endasm;

  __asm
        MOV  A,_data_epbuf
        ;; Bit0
        RRC  A
        MOV  _TDI,C
        SETB _TCK
        ;; Bit1
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        ;; Bit2
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        ;; Bit3
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        ;; Bit4
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        ;; Bit5
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        ;; Bit6
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        ;; Bit7
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        nop
        clr  _TCK
        ret
  __endasm;










  EA = 1;
}

void clock_bits_out_jtag()
{
    printf("Function currently unimplemented");
    mpsse_bits_clock_length = (get_next_byte()) + 1;
    data_epbuf = get_next_byte();
    printf("1b clocking %02x bits with value %02x\r\n",mpsse_bits_clock_length,data_epbuf);
     __asm
        MOV  A,_data_epbuf                  ;Move the data into the accumulator
        MOV r0,_mpsse_bits_clock_length
        0001$:RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        djnz r0,0001$
        nop
        CLR  _TCK
  __endasm;

}







void set_break_point()
{
    //This function busy waits if PB3 is high. The moment a start bit is detected, it
    //exits
    printf("Breakpoint");
    __asm
            //Like #define in C. Can easily be used to change the pin
        .equ _RX_PIN, _PB3
        clr _EA             //(2 cycles)
        0001$:
            jb _RX_PIN,0001$    //Wait for the bit to go low, busy wait until then
        setb _EA
    __endasm;
    printf("Released\r\n");
}

