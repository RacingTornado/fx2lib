#ifndef TIMER_LIB
#define TIMER_LIB


#define MAX_TIMERS 5

#define CKCON_T0 0x08
#define CKCON_T1 0x10
#define CKCON_T2 0x20
#define T0_MODE_REL 0x02

#define ENABLE_INTERRUPT()  EA = 1
#define DISABLE_INTERRUPT() EA = 0

#endif
