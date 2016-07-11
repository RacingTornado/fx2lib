/** \file include/buffer/buffer.h
 * Used for implementing the common API for accessing buffers on the FX2.
 **/
#ifndef BUFFER_BUFFER_H
#define BUFFER_BUFFER_H
#include "fx2types.h"
#include <fx2macros.h>
#include <stdio.h>

/**
 * \brief Stores the buffer number. This is incremented everytime the CREATE_BUFFER
 * macro is called. WARNING: Maximum number of buffers supported is 256. 
 **/
BYTE buffer_number;

/**
 * \brief Stores the current buffer number. Used to identify the buffer into which data was last inserted.
 **/
BYTE current_buffer_push;

/**
 * \brief Stores the current buffer number. Used to identify the buffer into which data was last removed.
 **/
BYTE current_buffer_pop;

/**
 * \brief Stores the current data. The data in the DPL is often over written because of the DPTR functions.
 **/
BYTE store_data;



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

/**
 * \brief Creates a buffer without using autopointers.
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

#define CREATE_BUFFER_AUTOPTR_SINGLE(name,size) 							\
	BYTE name##_offset;										\
	BYTE name##number;										\
	BYTE name##src;											\
	BYTE name##count;										\
	BYTE name##_sizeb;										\
	BYTE name##_tail_offset;									\
	__xdata BYTE name##_buffer[size];								\
	BOOL name##_init()										\
	{												\
		AUTOPTRSETUP =   bmAPTREN|bmAPTR1INC|bmAPTR2INC;					\
		LOADWORD(AUTOPTR1, &name##_buffer);							\
		LOADWORD(AUTOPTR2, &name##_buffer);							\
		name##number = buffer_number;								\
		name##_sizeb = size;									\
		name##src = MSB(&name##_buffer);							\
		name##_offset = LSB(&name##_buffer);							\
		name##_tail_offset = LSB(&name##_buffer);						\
		current_buffer_push = buffer_number;							\
		current_buffer_pop = buffer_number;							\
		buffer_number++;									\
		return TRUE;										\
	}												\
	BOOL name##_push(BYTE data)									\
	{												\
		__asm__("mov _store_data,dpl ;Store the value to be written to the buffer");		\
		__asm											\
		mov a, _##name##count									\
		cjne a,_##name##_sizeb,0002$								\
		ret;											\
		0002$:											\
		__endasm;										\
		__asm											\
		mov a,_##name##number									\
		cjne a,_current_buffer_push,0001$								\
		__endasm;										\
		put_data();										\
		__asm__("mov _" #name "_offset,_head_LSB");						\
		name##count++;										\
		if((&name##_buffer) + name##_sizeb == MAKEWORD(name##src,name##_offset))		\
		{											\
			printf("INsidexxxxxxxxxxxxxxxxxxxxxxxxx");					\
			name##src = MSB(&name##_buffer);						\
			name##_offset = LSB(&name##_buffer);						\
			__asm										\
			mov _head_MSB,_##name##src							\
			mov _head_LSB,_##name##_offset							\
			__endasm;									\
		}											\
		__asm											\
		ret ;Return from the function								\
		0001$:											\
		mov _current_buffer_push,_##name##number							\
		mov _head_MSB,_##name##src								\
		mov _head_LSB,_##name##_offset								\
		__endasm;										\
		printf("I shouldnt not be here");							\
		put_data();										\
		__asm__("mov _" #name "_offset,_head_LSB");						\
		name##count++;										\
		if((&name##_buffer) + name##_sizeb == MAKEWORD(name##src,name##_offset))		\
		{											\
			printf("INsidexxxxxxxxxxxxxxxxxxxxxxxxx");					\
			name##src = MSB(&name##_buffer);						\
			name##_offset = LSB(&name##_buffer);						\
			__asm										\
			mov _head_MSB,_##name##src							\
			mov _head_LSB,_##name##_offset							\
			__endasm;									\
		}											\
	}												\
	BYTE name##_pop()										\
	{												\
		__asm											\
		mov a, _##name##count									\
		jnz 0003$										\
		ret;											\
		0003$:											\
		mov a,_##name##number									\
		cjne a,_current_buffer_pop,0001$								\
		__endasm;										\
		return_data();										\
		name##count--;										\
		if((&name##_buffer) + (name##_sizeb-1) == MAKEWORD(name##src,name##_tail_offset))		\
		{											\
			printf("INsidexxxxfxxxxxxxxxxxxxxxxxxxxx");					\
			name##src = MSB(&name##_buffer);						\
			name##_tail_offset = LSB(&name##_buffer);					\
			__asm__("mov _tail_MSB, _" #name "src");							\
			__asm__("mov _tail_LSB,_" #name  "_tail_offset");							\
		}\										\
		__asm__("mov _" #name "_tail_offset,_tail_LSB");\
		__asm__("mov dpl,_store_data");\
		__asm\
		ret ;Return from the function								\
		0001$:											\
		mov _current_buffer_pop,_##name##number							\
		mov _tail_MSB,_##name##src								\
		mov _tail_LSB,_##name##_tail_offset								\
		__endasm;										\
		return_data();										\
		printf("I shouldnt not be here");							\
		__asm__("mov _" #name "_tail_offset,_tail_LSB");						\
		if((&name##_buffer) + name##_sizeb == MAKEWORD(name##src,name##_tail_offset))		\
		{											\
			printf("INsideyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy");				\
			name##src = MSB(&name##_buffer);						\
			name##_tail_offset = LSB(&name##_buffer);						\
			__asm										\
			mov _tail_MSB,_##name##src							\
			mov _tail_LSB,_##name##_tail_offset							\
			__endasm;									\
		}											\
		name##count--;										\
		__asm__("mov dpl,_store_data");\
	}												\
	BOOL name##_init();										\
	BOOL name##_push(BYTE data);									\
	BYTE name##_pop();

static inline void put_data()
{
	/*The first thing to do is check whether we need to reload the address pointer
	 *This handles the buffer_switch logic. That is a new buffer is being opened.	
	*/
		//printf("w %02x \r\n",head_LSB);

	__asm		
	mov	dptr,#_XAUTODAT1			;(Read data now)		
	mov	a,_store_data				;(push the data into the ACC)	
	movx	@dptr,a					;(Move the data back)	
	__endasm;				
}

static inline BYTE return_data()
{
		
	__asm
	mov	dptr,#_XAUTODAT2					//(3 cycles)
	movx	a,@dptr							//(1 cycle)
	mov _store_data,a							//(2 cycles)
	__endasm;
//printf("r%02x \r\n",store_data);
}
		
#endif
