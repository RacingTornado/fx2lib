/*
Remarks By Roaring(for the FX2LP from cypress)
*/

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>

void fast_uart(unsigned char a, unsigned char mask) {
   OEA |= mask;
   //An efficient UART bitbang routine in assembly
   __asm
   //Like #define in C. Can easily be used to change the pin
   .equ TX_PIN, _PA2
   //Disable interrupts
   clr _EA
   //Move the data to be sent into the ACC
   mov a , dpl
   //Clear carry
   clr c
   //We need to send out 8 bits of data
   //Load r0 with value 8
   mov r0, #0x08;
   //Create the start bit
   clr TX_PIN;
   //Precalculated delay since 1 cycle takes 88.33ns
   //Mov takes 2 cycles
   mov r1, #0x22;
   0006$:
   //DJNZ on Rn takes 3 cycle. This waits for
   //23*3, 69 cycles
   //71 cycle delay
   djnz r1, 0006$;
   //Add 2 more cycles of delay
   //97 cycles
   nop;
   nop;
   0001$:
   //2 cycles
   rrc a;
   //Move the carry into the port
   mov _PA2, c
   //Now we need to add delay for the next
   //2 cycles of delay
   mov r1, #0x1F;
   //31*3 , 93 cycles of delay
   0004$:
   djnz r1, 0004$;
   //1 more cycle, now upto 94
   nop;
   //3 more cycles of delay
   //97 cycles
   djnz r0, 0001$;
   setb _PA2;
   mov r1, #0x80;
   0005$:
   djnz r1, 0005$;
   nop
   setb _EA;
   __endasm;
}

