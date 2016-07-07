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
	__sfr __at 0x9a   name##_head_MSB; 					\
	__sfr __at 0x9b   name##_head_LSB; 					\
	__sfr __at 0x9d   name##_tail_MSB; 					\
	__sfr __at 0x9e   name##_tail_LSB;					\
	__sfr __at 0x9e   name##_tail_LSB;					\
	BYTE name##_size;								\
	__xdata BYTE name##_buffer[1<<size];						\
	BOOL name##_init()								\
	{										\
		AUTOPTRSETUP =   bmAPTREN|bmAPTR1INC|bmAPTR2INC;			\
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

static inline void get_data()
{
	XAUTODAT1 = 0x45;
}

static inline BYTE return_data()
{
	__asm
	mov	dptr,#_XAUTODAT2
	movx	a,@dptr
	mov dpl,a
	__endasm;
}		
#endif
