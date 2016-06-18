#include <stdio.h>

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>
#include <delay.h>
#include <autovector.h>
#include <lights.h>
#include <setupdat.h>
#include <eputils.h>
#include <fx2ints.h>
#include "softuart.h"
#include "mpsse.h"

extern void toggle_pins();
unsigned char k;

BOOL handle_mpsse()
{

   EP0BCH = 0;
   EP0BCL = 3;

   for (k = 0; k < 8; k++)
   {
      //softuart_putchar(SETUPDAT[k]);
   }

   for (k = 0; k < 10; k++)
   {
      //softuart_putchar(EP0BUF[k]);
   }

   //SUDPTRCTL = 0x01;
   switch (SETUPDAT[1])
   {
      case MPSSE_BITBANG:
      {
         //EP0CS |= 0x80;
         //toggle_pins();
         //toggle_pins();
         EP0BUF[0] = 01;
         EP0BUF[1] = 06;
         EP0BCH = 0;
         EP0BCL = 2;
         //SUDPTRL = 03;
         //EP0CS = EP0CS & 0x7f;
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
         return FALSE;
   }

   return TRUE;
}
