#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"
#include "mpsse_utils.h"

__xdata __at(0xE6B8) volatile struct mpsse_control_request control_request;
__xdata __at(0xF000) volatile struct mpsse_read_write read_write;
__xdata enum mpsse_clocking_commands clocking_commands;
__xdata struct mpsse_total_length total_length;
enum mpsse_isr_state isr_state;
enum mpsse_isr_mode isr_mode;
unsigned char bit_count;

void uart_tx(char c);

 void mpsse_reset()
 {
     //Initialize to IDLE state.
     isr_state = IDLE;
     //Number of bits which should be shifted in or out.
     bit_count = 0;
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
    switch(read_write.command)
    {
    case SET_BITS_LOW:
        //Look again and verify that this can actually be done
        OEA = read_write.direction;
        IOA = read_write.value;
        printf("Write direction %02x, value %02x\r\n",read_write.direction,read_write.value);
        break;
    case SET_BITS_HIGH:
        OEB = read_write.direction;
        IOB = read_write.value;
        printf("Write high bytes\r\n");
        break;
    case GET_BITS_LOW:
        printf("Read low bytes\r\n");
        break;
    case GET_BITS_HIGH:
        printf("Read high bytes\r\n");
        break;
    default:
        break;
    }
}


