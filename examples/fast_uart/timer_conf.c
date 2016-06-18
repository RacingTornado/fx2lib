#include <fx2regs.h>
#include <fx2macros.h>
#include <fx2ints.h>
#include <autovector.h>
#include <delay.h>
#include <setupdat.h>

#define SYNCDELAY SYNCDELAY4

#define BAUD_57600 57600
#define BAUD_38400 38400
#define BAUD_28800 28800
#define BAUD_19200 19200
#define BAUD_9600  9600
#define BAUD_4800  4800
#define BAUD_2400  2400

static BYTE reload_value = 0;
extern void toggle_pins();

void conf_timer(DWORD baud, CLK_SPD cpu_clk)
{
   switch (cpu_clk)
   {
      case CLK_12M:
         switch (baud)
         {
            case BAUD_57600:
               reload_value = 0xfd;
               break;

            case BAUD_38400:
               reload_value = 0xfb;
               break;

            case BAUD_28800:
               reload_value = 0xf9;
               break;

            case BAUD_19200:
               reload_value = 0xf6;
               break;

            case BAUD_9600:
               reload_value = 0xec;
               break;

            case BAUD_4800:
               reload_value = 0xd9;
               break;

            case BAUD_2400:
               reload_value = 0xb2;
               break;

            default:
               reload_value = 0x00;
         }

      case CLK_24M:
      {
         switch (baud)
         {
            case BAUD_57600:
               reload_value = 0xf9;
               break;

            case BAUD_38400:
               reload_value = 0xf6;
               break;

            case BAUD_28800:
               reload_value = 0xf3;
               break;

            case BAUD_19200:
               reload_value = 0xec;
               break;

            case BAUD_9600:
               reload_value = 0xd9;
               break;

            case BAUD_4800:
               reload_value = 0xb2;
               break;

            case BAUD_2400:
               reload_value = 0x64;
               break;

            default:
               reload_value = 0;
         }
      }

      case CLK_48M:
      {
         switch (baud)
         {
            case BAUD_57600:
               reload_value = 0xf3;
               break;

            case BAUD_38400:
               reload_value = 0xec;
               break;

            case BAUD_28800:
               reload_value = 0xe6;
               break;

            case BAUD_19200:
               reload_value = 0xd9;
               break;

            case BAUD_9600:
               reload_value = 0xb2;
               break;

            case BAUD_4800:
               reload_value = 0x64;
               break;

            case BAUD_2400:
               reload_value = 0;
               break;

            default:
               reload_value = 0x00;
         }
      }
   }
}




void configure_timer() __critical
{
   TMOD |= 0x20;
   SYNCDELAY;
   TR1 = 0;
   SYNCDELAY;
   TH1 = 0xc3;
   SYNCDELAY;
   TL1 = 0x23;
   SYNCDELAY;


}



void start_timer()
{
   TR1 = 1;
   SYNCDELAY;
}



