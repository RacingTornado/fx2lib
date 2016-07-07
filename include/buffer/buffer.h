/** \file include/buffer/buffer.h
 * Used for implementing the common API for accessing buffers on the FX2.
 **/
#ifndef BUFFER_BUFFER_H
#define BUFFER_BUFFER_H
#include "fx2types.h"
#include <fx2macros.h>

/**
 * \brief Stores the buffer number. This is incremented everytime the CREATE_BUFFER
 * macro is called. WARNING: Maximum number of buffers supported is 256. 
 **/
BYTE buffer_number;

/**
 * \brief Stores the current buffer number. Used to identify which buffer switched last.
 **/
BYTE current_buffer;



__sfr __at 0x9a   head_MSB;
__sfr __at 0x9b   head_LSB;
__sfr __at 0x9d   tail_MSB;
__sfr __at 0x9e   tail_LSB;


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
	BYTE name##_offset;								\
	BYTE name##number;								\
	BYTE name##src;									\
	__xdata BYTE name##_buffer[1<<size];						\
	BOOL name##_init()								\
	{										\
		AUTOPTRSETUP =   bmAPTREN|bmAPTR1INC|bmAPTR2INC;			\
		LOADWORD(AUTOPTR1, &name##_buffer);					\
		LOADWORD(AUTOPTR2, &name##_buffer);					\
		name##number = buffer_number;						\
		buffer_number++;							\
		return TRUE;								\
	}										\
	BOOL name##_push(BYTE data)							\
	{										\
		__asm									\
		mov a,_##name##number ;Move the buffer number				\
		cjne a,_current_buffer,0001$		;Check the last accessed buffer	\
		__endasm;								\
		put_data();								\
		__asm									\
		ret ;Return from the function						\
		0001$:									\
		mov _current_buffer,_##name##number	;(Load buffer in use)		\
		mov _head_MSB,_##name##src		;(Reload the MSB location)	\
		mov _head_LSB,_##name##_offset		;(Reload the LSB location)	\
		__endasm;								\
											\
	}										\
	BYTE name##_pop()								\
	{										\
		return_data();								\
	}										\
	BOOL name##_init();								\
	BOOL name##_push(BYTE data);							\
	BYTE name##_pop();

static inline void put_data()
{
	/*The first thing to do is check whether we need to reload the address pointer
	 *This handles the buffer_switch logic. That is a new buffer is being opened.	
	*/	
	__asm				
	mov	dptr,#_XAUTODAT1		;(Read data now)		
	mov	a,#0x35				;(push the data into the ACC)	
	movx	@dptr,a				;(Move the data back)	
	__endasm;				
}

static inline BYTE return_data()
{
	__asm
	mov	dptr,#_XAUTODAT2					//(3 cycles)
	movx	a,@dptr							//(1 cycle)
	mov dpl,a							//(2 cycles)
	__endasm;
}		
#endif
