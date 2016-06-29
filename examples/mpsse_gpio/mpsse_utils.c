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


   EP0BCH = 0;
   EP0BCL = 3;
   //SUDPTRCTL = 0x01;
   uart_tx(SETUPDAT[1]);
   switch (SETUPDAT[1])
   {
      case MPSSE_BITBANG:
      {

         EP0BUF[0] = 01;
         EP0BUF[1] = 06;
         EP0BCH = 0;
         EP0BCL = 2;
         EP0CS |= 0x80;
      }
      break;

      case MPSSE_TWO:
      {
         EP0CS |= 0x80;
         EP0BUF[0] = 1;
         EP0BUF[1] = 2;
         EP0BCH = 0;
         EP0BCL = 2;
         SUDPTRL = 2;
      }
      break;
      case MPSSE_THREE:
      {
         EP0BUF[0] = 1;
         EP0BUF[1] = 2;
         EP0BCH = 0;
         EP0BCL = 2;
         SUDPTRL = 2;
         EP0CS |= 0x80;
      }
      break;
      case MPSSE_FOUR:
      {
         EP0BUF[0] = 0;
         EP0BUF[1] = 1;
         EP0BCH = 0;
         EP0BCL = 2;
         SUDPTRL = 3;
         EP0CS |= 0x80;
      }
      break;
      case MPSSE_FIVE:
      {
         EP0BUF[0] = 1;
         EP0BUF[1] = 0;
         EP0BCH = 0;
         EP0BCL = 1;
         SUDPTRL = 3;
         EP0CS |= 0x80;
      }
      break;
      case MPSSE_SIX:
      {
         EP0BUF[0] = 0;
         EP0BUF[1] = 1;
         EP0BCH = 0;
         EP0BCL = 1;
         SUDPTRL = 3;
         EP0CS |= 0x80;
      }
      break;
      default:
         break;
   }

}
