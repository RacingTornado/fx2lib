#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"
#include "mpsse_utils.h"

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
    unsigned char a;
    unsigned char b;
    //Points to the endpoint 2 FIFO buffer
    ep2_buffer.DAT = EP2FIFOBUF;
    //Store the total length of bytes received
    ep2_buffer.total_length = EP2BCL | (EP2BCH << 8);
    ep2_buffer.current_index = 65535;
    printf("Length is %02d\r\n",ep2_buffer.total_length);
    while(ep2_buffer.total_length!=0)
    {
        get_next_byte();
        printf("Current byte is %02x and length is %02d\r\n",get_current_byte(),ep2_buffer.current_index);
        switch(get_current_byte())
        {
        case SET_BITS_LOW:
            //Look again and verify that this can actually be done
            a = get_next_byte();
            b = get_next_byte();
            IOA = a;
            OEA = b;
            OEA = b;
            //decrement_total_byte_count(3);
            //printf("Write direction %02x, value %02x length %02d\r\n",a,b, ep2_buffer.total_length);
            break;
        case SET_BITS_HIGH:
            OEB = get_next_byte();
            IOB = get_next_byte();
            //decrement_total_byte_count(3);
            //printf("Write high bytes\r\n");
            break;
        case GET_BITS_LOW:
            //decrement_total_byte_count(1);
            //printf("Read low bytes\r\n");
            break;
        case GET_BITS_HIGH:
            //decrement_total_byte_count(1);
            //printf("Read high bytes\r\n");
            break;
        case   CLOCK_BYTES_OUT_POS_MSB:
            clock_obyte_data_pos(0);
            break;
        case CLOCK_BYTES_OUT_NEG_MSB:
            clock_obyte_data_neg(0);
            break;
        case CLOCK_BITS_OUT_POS_MSB:
            clock_obits_data_pos(0);
            break;
        case CLOCK_BITS_OUT_NEG_MSB:
            clock_obits_data_neg(0);
            break;
        case CLOCK_BYTES_IN_POS_MSB:
            clock_ibyte_data_pos(0);
            break;
        case CLOCK_BYTES_IN_NEG_MSB:
            clock_ibyte_data_neg(0);
            break;
        case CLOCK_BITS_IN_POS_MSB:
            clock_ibits_data_pos(0);
            printf("Now length is %02x current byte is %02x",get_current_length(),get_current_byte());
            break;
        case CLOCK_BITS_IN_NEG_MSB:
            clock_ibits_data_neg(0);
            break;
        case CLOCK_BYTES_IN_OUT_NORMAL_MSB:
            clock_iobyte_data(0,0);
            break;
        case CLOCK_BYTES_IN_OUT_INVERTED_MSB:
            clock_iobyte_data(1,0);
            break;
        case CLOCK_BITS_IN_OUT_NORMAL_MSB:
            clock_iobits_data(0,0);
            break;
        case CLOCK_BITS_IN_OUT_INVERTED_MSB:
            clock_iobits_data(1,0);
            break;
        case CLOCK_BYTES_OUT_POS_LSB:
            clock_obyte_data_pos(1);
            break;
        case CLOCK_BYTES_OUT_NEG_LSB:
            clock_obyte_data_neg(1);
            break;
        case CLOCK_BITS_OUT_POS_LSB:
            clock_obits_data_pos(1);
            break;
        case CLOCK_BITS_OUT_NEG_LSB:
            clock_obits_data_neg(1);
            break;
        case CLOCK_BYTES_IN_POS_LSB:
            clock_ibyte_data_pos(1);
            break;
        case CLOCK_BYTES_IN_NEG_LSB:
            clock_ibyte_data_neg(1);
            break;
        case CLOCK_BITS_IN_POS_LSB:
            clock_ibits_data_pos(1);
            break;
        case CLOCK_BITS_IN_NEG_LSB:
            clock_ibits_data_neg(1);
            break;
        case CLOCK_BYTES_IN_OUT_NORMAL_LSB:
             read_write_bytes_JTAG();
            //clock_iobyte_data(0,1);
            break;
        case CLOCK_BYTES_IN_OUT_INVERTED_LSB:
            clock_iobyte_data(1,1);
            break;
        case CLOCK_BITS_IN_OUT_NORMAL_LSB:
            read_write_bits_JTAG();
            //clock_iobits_data(0,1);
            break;
        case CLOCK_BITS_IN_OUT_INVERTED_LSB:
            clock_iobits_data(1,1);
            break;
        case SEND_IMMEDIATE:
            send_endpoint_flush(0);
            break;
        case CLOCK_DATA_TMS_NEG:
            /* The next 2 bytes indicate the number of bits to clock out via TMS*/
            clock_bits_tms();
            break;
        case CLOCK_DATA_TMS_WITH_READ:
            read_bits_write_TMS_JTAG();
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
    //decrement_total_byte_count(3);
    //ep2_buffer.total_length = ep2_buffer.total_length - (mpsse_byte_clock_length);
    while(mpsse_byte_clock_length!=0)
    {
        if(isr_state != BUSY)
        {

            mpsse_isr_buffer = get_next_byte();
            printf("Clocking out %02x\r\n",mpsse_isr_buffer);
            mpsse_bit_count  = 0x09;
            isr_state  = BUSY;
            isr_mode   = TX;
            while(isr_state  == BUSY);
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
        EP1INBUF[2] = 0x00;
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
    if(ep2_buffer.current_index == 511 )
    {
        //Rearm the endpoint
        EP2BCL = 0x80; // write once
        //Decrement the isr counter
        SYNCDELAY;
        isr_enter -- ;
        ep2_buffer.total_length = EP2BCL | (EP2BCH << 8);
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

__sbit __at 0x82         TDO; /* Port A.1 */


/* JTAG TMS, AS/PS nCONFIG */

__sbit __at 0x81          TDI; /* Port A.2 */


/* JTAG TDO, AS/PS CONF_DONE */

__sbit __at 0x83          TMS; /* Port A.3 */




//-----------------------------------------------------------------------------

void ProgIO_ShiftOut(unsigned char c)
{
  /* Shift out byte C:
   *
   * 8x {
   *   Output least significant bit on TDI
   *   Raise TCK
   *   Shift c right
   *   Lower TCK
   * }
   */

  (void)c; /* argument passed in DPL */

  __asm
        MOV  A,DPL                  ;Move the data into the accumulator
        RRC  A                      ;Rotate the accumulator right
        MOV  _TDI,C                 ;Move the value into TDI
        SETB _TCK                   ;Set the clock high, keep repeating.
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        RRC  A
        CLR  _TCK
        MOV  _TDI,C
        SETB _TCK
        nop
        CLR  _TCK
        ret
  __endasm;
}

/*
;; For ShiftInOut, the timing is a little more
;; critical because we have to read _TDO/shift/set _TDI
;; when _TCK is low. But 20% duty cycle at 48/4/5 MHz
;; is just like 50% at 6 Mhz, and that's still acceptable
*/

unsigned char read_write_bits_JTAG()
{
  /* Shift out byte C, shift in from TDO:
   *
   * 8x {
   *   Read carry from TDO
   *   Output least significant bit on TDI
   *   Raise TCK
   *   Shift c right, append carry (TDO) at left
   *   Lower TCK
   * }
   * Return c.
   */
    printf("Shifting data in and out %02x\r\n",get_current_byte());
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
        MOV  DPL,A
        ret
  __endasm;

  /* return value in DPL */

  return ;
}


void shift_bytes_JTAG()
{
  /* Shift out byte C, shift in from TDO:
   *
   * 8x {
   *   Read carry from TDO
   *   Output least significant bit on TDI
   *   Raise TCK
   *   Shift c right, append carry (TDO) at left
   *   Lower TCK
   * }
   * Return c.
   */
    printf("Shifting data in and out for JTAG %02x\r\n",get_current_byte());
    data_epbuf = get_next_byte();
    EA = 0;
  __asm
        nop
        nop
        nop
        nop
        MOV  A,_data_epbuf
        mov r0,#0x08
        0001$:
        MOV  C,_TDO
        RRC  A
        MOV  _TDI,C
        SETB _TCK
        nop
        nop
        nop
        nop
        CLR  _TCK
        djnz r0, 0001$                  ;Stop if 8 bits have been shifted, we have to reload buffers.
        mov _mpsse_isr_buffer,a         ;Move the data into the buffer to be read
        ret
  __endasm;
  EA = 1;
}



void clock_bits_tms()
{

    mpsse_bits_clock_length = (get_next_byte()) + 1;
    data_epbuf = get_next_byte();
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
        ret
  __endasm;


}

unsigned char read_bits_write_TMS_JTAG()
{

    printf("Read TDI, clock out TDO %02x\r\n",get_current_byte());
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
        MOV  DPL,A
        ret
  __endasm;

}


void read_write_bytes_JTAG()
{

        /* The command has been read. The next 2 bytes gives us the
     * the number of bytes we need to read .
     */
    printf("Reading and writing bytes from JTAG %02x\r\n",get_current_byte());
    mpsse_byte_clock_length = (get_next_byte() | (get_next_byte()<<8)) + 1;
    //decrement_total_byte_count(2);
    //Initialize to 0 once we enter.
    while(mpsse_byte_clock_length!=0)
    {


            //mpsse_isr_buffer = 0x00;
            //mpsse_bit_count  = 0x09;
            shift_bytes_JTAG();
            mpsse_byte_clock_length = mpsse_byte_clock_length - 1;
            //isr_state  = BUSY;
            //isr_mode   = RX;
            //delete_packets_read++;
            //while(isr_state  == BUSY);
            //printf("Packets read is %02d\r\n",delete_packets_read);
            put_ep1in_data();
    }
}







