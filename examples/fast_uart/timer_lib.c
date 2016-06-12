/*

Most of the logic here is implemented from this http://www.simplyembedded.org/tutorials/timers/

*/

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
#include "timer_lib.h"

#define SYNCDELAY SYNCDELAY4


extern void fast_uart(unsigned char a);

struct timer
{
   //The number of ticks after which the task should fire.
   //This keeps getting incremented until it finally rolls over and starts back
   //A short is about 2 bytes on SDCC. So 65535 is the maximum count it can measure.
   unsigned char expiry;
   //The period after which the timer should fire. This field is a constant
   unsigned char periodic;
   void (*callback)();
};

__xdata struct timer fx2_timer[MAX_TIMERS];
//Incremented via ISR
__xdata  volatile unsigned short fx2_tick = 0;
static __xdata unsigned char to_load;
__xdata unsigned char interval;
__xdata unsigned char i;
__xdata short periodic;
void (*callback)();

void timerlib_init(CLK_SPD clk)
{
   interval = 100;
   TR0 = 0x00;
   //CKCON = 0x08;
   TMOD  = T0_MODE_REL;

   switch (clk)
   {
      case CLK_12M:
      {
         to_load = (interval) / (83 * 4 * 3);
         to_load = 0xff - to_load;
      }
      break;

      case CLK_24M:
      {
         to_load = (interval * 1000) / (83 * 2 * 3);
         to_load = 0xff - to_load;
      }
      break;

      case CLK_48M:
      {
         to_load = (interval);
         to_load = 0xff - to_load;
      }
      break;

      default:
      {
      }
   }

   //We manually load right now. This will be updated in the next version
   TH0 = 0x30;
   TL0 = 0x30;

}



void create_timer()

{
   //The following parameters need to be set before a call to this function.
   //int periodic, void (*callback)(void *)
   /* Find a free timer */
   for (i = 0; i < MAX_TIMERS; i++)
   {
      if (fx2_timer[i].callback == NULL)
      {
         break;
      }
   }

   /* Make sure a valid timer is found */
   if (i < MAX_TIMERS)
   {
      EA = 0;

      /* Set up the timer */
      //The periodic holds the number of ticks after which the timer should fire.
      if (periodic != 0)
      {
         fx2_timer[i].periodic = periodic ;
      }
      else
      {
         fx2_timer[i].periodic = 0;
      }

      fx2_timer[i].callback = callback;
      //From the point it is called, we load the value of expiry
      fx2_timer[i].expiry = fx2_tick + fx2_timer[i].periodic;
      EA = 1;
   }
}

//Delete timer which has type handle.
void delete_timer(__xdata unsigned char handle)
{
   if (handle < MAX_TIMERS)
   {
      EA = 0;
      /* Clear the callback to delete the timer */
      fx2_timer[handle].callback = NULL;
      EA = 1;
   }
}



void timer_interrupt_enable()
{
   ENABLE_TIMER0();
}

void timer_start()
{
   TCON |= 0x10;
   SYNCDELAY;
   ENABLE_TIMER0();
   SYNCDELAY;
}

void service_timer()
{
   //fast_uart(0x34);
   for (i = 0; i < MAX_TIMERS; i++)
   {
      /* If the timer is enabled and expired, invoke the callback */
      //If a valid callback exists, and the value of count in the timer expiry
      //has reached timer_tick. This may not always work, so we change the value to greater than
      if ((fx2_timer[i].callback != NULL) && ( fx2_tick > fx2_timer[i].periodic))
      {
         fx2_tick = 0;
         fx2_timer[i].callback();
         //fast_uart(0x47);
         /* Timer is periodic, calculate next expiration */
         //fast_uart(fx2_timer[i].expiry);
         EA = 0;
         fx2_timer[i].expiry = fx2_timer[i].periodic + fx2_tick;
         EA = 1;
         //fast_uart(fx2_timer[i].expiry);
      }
   }
}
