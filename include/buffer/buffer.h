/** \file include/buffer/buffer.h
 * Used for implementing the common API for accessing buffers on the FX2.
 **/
#ifndef BUFFER_BUFFER_H
#define BUFFER_BUFFER_H
#include "fx2types.h"
#include <fx2macros.h>


/**
 * \brief Creates a buffer.
 * \li name##_head - This is a single byte which stores the  location of the head pointer. 
 * \li name##_tail - This is a single byte which stores the  location of the tail pointer.
 * \li name##_size - The size of the buffer.
 * \li name##_buffer - The actual buffer.
 **/
#define CREATE_BUFFER(name, type, size) \
	volatile BYTE name##_head; \
	volatile __bit name##_head_inc; \
	volatile BYTE name##_tail; \
	type BYTE name##_buffer[1<<size];

#define CREATE_BUFFER_AUTOPTR_SINGLE(name,size) 					\
	__sfr __at 0x9a volatile BYTE ##name_head_MSB; 					\
	__sfr __at 0x9b volatile BYTE ##name_head_LSB; 					\
	__sfr __at 0x9d volatile BYTE ##name_tail_MSB; 					\
	__sfr __at 0x9e volatile BYTE ##name_tail_LSB;					\
	__sfr __at 0x9e volatile BYTE ##name_tail_LSB;					\
	BYTE name##_size;								\
	__xdata BYTE name##_buffer[1<<size];						\
	BOOL name##_init()								\
	{										\
		AUTOPTRSETUP = bmAPTR2INC | bmAPTR1INC | bmAPTREN;			\
		LOADWORD(AUTOPTR1, &name##_buffer);					\
		LOADWORD(AUTOPTR2, &name##_buffer);					\
		return TRUE;								\
	}										\
	BOOL name##_push(BYTE data)							\
	{										\
		get_data();								\
	}										\
	BYTE name##_pop()								\
	{										\
		return_data();								\
	}										\
	BOOL name##_init();								\
	BOOL name##_push(BYTE data);							\
	BYTE name##_pop();

static inline get_data()
{
	__asm
	mov a,#0x45 							
	mov dptr,#0xE67B							
	movx @dptr,a
	__endasm;
}

static inline return_data()
{
	__asm									
	mov dptr,#0xE67C							
	movx a,@dptr								
	mov dpl,a								
	__endasm;
}		
#endif
