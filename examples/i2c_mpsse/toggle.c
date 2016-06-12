#include <fx2macros.h>
#include <fx2ints.h>
#include <autovector.h>
#include <delay.h>
#include <setupdat.h>


#define SYNCDELAY SYNCDELAY4


void configure_drive(unsigned char a, unsigned char b)
{
   switch (a)
   {
      case 0x80:
         OEA |= b;
         SYNCDELAY;
         break;

      case 0x90:
         OEB |= b;
         SYNCDELAY;
         break;

      case 0xa0:
         OEC |= b;
         SYNCDELAY;
         break;

      case 0xb0:
         OED |= b;
         __asm
         mov 0xb5, #0x01
         __endasm;
         SYNCDELAY;
         break;

      case 0xb1:
            OEE |= b;
         SYNCDELAY;
         break;
   }
}



void set_port_value(unsigned char a, unsigned char b)
{
   switch (a)
   {
      case 0x80:
            IOA |= b;
         SYNCDELAY;
         break;

      case 0x90:
            IOB |= b;
         SYNCDELAY;
         break;

      case 0xa0:
            IOC |= b;
         SYNCDELAY;
         break;

      case 0xb0:
            IOD |= b;
         SYNCDELAY;
         break;

      case 0xb1:
            IOE |= b;
         SYNCDELAY;
         break;
   }
}


void toggle_port_value(unsigned char a, unsigned  char b)
{
   switch (a)
   {
      case 0x80:
            IOA ^= b;
         SYNCDELAY;
         break;

      case 0x90:
            IOB ^= b;
         SYNCDELAY;
         break;

      case 0xa0:
            IOC ^= b;
         SYNCDELAY;
         break;

      case 0xb0:
            IOD ^= b;
         SYNCDELAY;
         break;

      case 0xb1:
            IOE ^= b;
         SYNCDELAY;
         break;
   }
}
