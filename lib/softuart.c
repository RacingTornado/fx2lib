/*
Remarks By Roaring(for the FX2LP from cypress)
*/

#include <fx2regs.h>
#include <fx2macros.h>
#include <serial.h>








void putchar(char c) {

   //Done in ASM to improve performance. It takes only 6 
   //cycles to move the data out, however a delay has been
   //introduced in order to get a baud rate of 115200
   //The mask which is to be written into the pin
   OEA |= 0x04;
   //An efficient UART bitbang routine in assembly
   __asm
   //Like #define in C. Can easily be used to change the pin
   .equ TX_PIN, _PA2
   //Disable interrupts 
   //This is used because timing is critical
   //If the FX2 jumps into the ISR temporarily , it may cause transmit
   //errors. By clearing EA, we can disable interrupts
   clr _EA
   //Move the data to be sent into the ACC
   //The data which is to be shifted out is held in the dpl register
   //We move the data into A for easy access to subsequent instructions
   mov a , dpl
   //Clear carry. This takes 1 cycle
   clr c 
   //We need to send out 8 bits of data
   //Load r0 with value 8
   //This instruction takes 2 cycles
   mov r0, #0x08;
   //Create the start bit 
   //Takes about 2 cycles
   clr TX_PIN;
   //Precalculated delay since 1 cycle takes 88ns
   //At 12Mhz, it should be about 83.33ns
   //But it appears to be about 88ns
   //These numbers have been verified using an analyzer
   //This takes about 2 cycles
   mov r1, #0x20;
   0006$:
   //DJNZ on Rn takes 3 cycle 
   //1 bit is about 8.6us
   djnz r1, 0006$;
   //NOP takes about 1 cycle
   //Add 2 more cycles of delay
   //97 cycles 
   nop; 
   nop;
   0001$:
   //2 cycles
   rrc a;
   //Move the carry into the port 
   //Takes another 2 cycles
   mov _PA2, c
   //Now we need to add delay for the next
   //2 cycles of delay
   mov r1, #0x1F;
   //31*3 , 93 cycles of delay
   0004$:
   //DJNZ takes about 3 cycles
   djnz r1, 0004$;
   //1 more cycle, now upto 94
   nop;
   //3 more cycles of delay
   //97 cycles
   djnz r0, 0001$;
   setb _PA2;
   //For the stop bit
   //We need to delay the stop bit
   //for 97 cycles also.
   mov r1, #0x20;
   0005$:
   djnz r1, 0005$;
   nop
   setb _EA;
   __endasm;


}







