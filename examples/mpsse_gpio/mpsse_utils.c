#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"

__xdata __at(0xE6B8) volatile struct mpsse_control_request mpsse_control;


void uart_tx(char c);

#define MPSSE_BITBANG 0x00
#define VENDOR_COMMAND 0x40
#define MPSSE_TWO 0x03
#define MPSSE_THREE 0x09
#define MPSSE_FOUR 0x0b
#define MPSSE_FIVE 0x0c
#define MPSSE_SIX 0x06

 void mpsse_parse_control()
{




   switch (SETUPDAT[1])
   {
      case MPSSE_BITBANG:
      {
         EP0CS |= 0x80;
      }
      break;

      case MPSSE_TWO:
      {
         EP0CS |= 0x80;

      }
      break;
      case MPSSE_THREE:
      {
         EP0CS |= 0x80;
      }
      break;
      case MPSSE_FOUR:
      {
         EP0CS |= 0x80;
      }
      break;
      case MPSSE_FIVE:
      {
         EP0CS |= 0x80;
      }
      break;
      case MPSSE_SIX:
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
