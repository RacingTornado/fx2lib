#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"
#include "mpsse_utils.h"

__xdata __at(0xE6B8) volatile struct mpsse_control_request control_request;


void uart_tx(char c);

void mpsse_parse_control()
{
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
