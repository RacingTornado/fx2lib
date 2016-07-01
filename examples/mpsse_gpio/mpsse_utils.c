#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"
#include "mpsse_utils.h"

__xdata __at(0xE6B8) volatile struct mpsse_control_request control_request;
__xdata __at(0xF000) volatile struct mpsse_read_write read_write;
__xdata enum mpsse_clocking_commands clocking_commands;
__xdata struct mpsse_ep2_buffer ep2_buffer;
enum mpsse_isr_state isr_state;
enum mpsse_isr_mode isr_mode;
unsigned char mpsse_bit_count;
unsigned char mpsse_isr_buffer;

void uart_tx(char c);

 void mpsse_reset()
 {
     //Initialize to IDLE state.
     isr_state = IDLE;
     //Number of bits which should be shifted in or out.
     mpsse_bit_count = 0;
     isr_mode  = TX;
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
    SYNCDELAY;
    /*Arm the endpoint*/
    EP2BCL = 0xff;

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
        printf("Length is %02d\r\n",ep2_buffer.total_length);
    switch(get_next_byte())
    {
    case SET_BITS_LOW:
        //Look again and verify that this can actually be done
        a = get_next_byte();
        b = get_next_byte();
        OEA = b;
        IOA = a;
        ep2_buffer.total_length = ep2_buffer.total_length - 3;
        printf("Write direction %02x, value %02x length %02d\r\n",a,b, ep2_buffer.total_length);
        break;
    case SET_BITS_HIGH:
        OEB = get_next_byte();
        IOB = get_next_byte();
        ep2_buffer.total_length = ep2_buffer.total_length - 3;
        printf("Write high bytes\r\n");
        break;
    case GET_BITS_LOW:
        printf("Read low bytes\r\n");
        break;
    case GET_BITS_HIGH:
        printf("Read high bytes\r\n");
        break;
    case   CLOCK_BYTES_OUT_POS_MSB:
        clock_obyte_data_pos(ep2_buffer.current_index,0);
        break;
    case CLOCK_BYTES_OUT_NEG_MSB:
        clock_obyte_data_neg(ep2_buffer.current_index,0);
        break;
    case CLOCK_BITS_OUT_POS_MSB:
        clock_obits_data_pos(ep2_buffer.current_index,0);
        break;
    case CLOCK_BITS_OUT_NEG_MSB:
        clock_obits_data_neg(ep2_buffer.current_index,0);
        break;
    case CLOCK_BYTES_IN_POS_MSB:
        clock_ibyte_data_pos(ep2_buffer.current_index,0);
        break;
    case CLOCK_BYTES_IN_NEG_MSB:
        clock_ibyte_data_neg(ep2_buffer.current_index,0);
        break;
    case CLOCK_BITS_IN_POS_MSB:
        clock_ibits_data_pos(ep2_buffer.current_index,0);
        break;
    case CLOCK_BITS_IN_NEG_MSB:
        clock_ibits_data_neg(ep2_buffer.current_index,0);
        break;
    case CLOCK_BYTES_IN_OUT_NORMAL_MSB:
        clock_iobyte_data(ep2_buffer.current_index,0,0);
        break;
    case CLOCK_BYTES_IN_OUT_INVERTED_MSB:
        clock_iobyte_data(ep2_buffer.current_index,1,0);
        break;
    case CLOCK_BITS_IN_OUT_NORMAL_MSB:
        clock_iobits_data(ep2_buffer.current_index,0,0);
        break;
    case CLOCK_BITS_IN_OUT_INVERTED_MSB:
        clock_iobits_data(ep2_buffer.current_index,1,0);
        break;
    case CLOCK_BYTES_OUT_POS_LSB:
        clock_obyte_data_pos(ep2_buffer.current_index,1);
        break;
    case CLOCK_BYTES_OUT_NEG_LSB:
        clock_obyte_data_neg(ep2_buffer.current_index,1);
        break;
    case CLOCK_BITS_OUT_POS_LSB:
        clock_obits_data_pos(ep2_buffer.current_index,1);
        break;
    case CLOCK_BITS_OUT_NEG_LSB:
        clock_obits_data_neg(ep2_buffer.current_index,1);
        break;
    case CLOCK_BYTES_IN_POS_LSB:
        clock_ibyte_data_pos(ep2_buffer.current_index,1);
        break;
    case CLOCK_BYTES_IN_NEG_LSB:
        clock_ibyte_data_neg(ep2_buffer.current_index,1);
        break;
    case CLOCK_BITS_IN_POS_LSB:
        clock_ibits_data_pos(ep2_buffer.current_index,1);
        break;
    case CLOCK_BITS_IN_NEG_LSB:
        clock_ibits_data_neg(ep2_buffer.current_index,1);
        break;
    case CLOCK_BYTES_IN_OUT_NORMAL_LSB:
        clock_iobyte_data(ep2_buffer.current_index,0,1);
        break;
    case CLOCK_BYTES_IN_OUT_INVERTED_LSB:
        clock_iobyte_data(ep2_buffer.current_index,1,1);
        break;
    case CLOCK_BITS_IN_OUT_NORMAL_LSB:
        clock_iobits_data(ep2_buffer.current_index,0,1);
        break;
    case CLOCK_BITS_IN_OUT_INVERTED_LSB:
        clock_iobits_data(ep2_buffer.current_index,1,1);
        break;
    default:
        ep2_buffer.total_length = ep2_buffer.total_length - 1;
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

void clock_obyte_data_pos(unsigned char offset, __bit dir)
{
    unsigned char waiter;
    if(isr_state == IDLE || isr_state == COMPLETE)
    {
    OEA = 0x03;
    for(waiter = 0 ; waiter <0xff; waiter++)
    {
        __asm
        mul ab
        mul ab
        mul ab
        mul ab
        __endasm;

    }
    isr_state = BUSY;
    mpsse_isr_buffer = 0x23;
    mpsse_bit_count = 0x09;
    }

}

