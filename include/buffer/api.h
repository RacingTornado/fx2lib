/** \file include/buffer/api.h
 * Used for defining a common API for accessing buffers on the FX2.
 **/

#ifndef BUFFER_API_H
#define BUFFER_API_H
#include "fx2types.h"

/**
 * \brief Initializes a buffer.
 **/
BOOL bufferX_init();

/**
 * \brief Gets the next byte from the UART buffer if the buffer is not empty.
**/
BYTE bufferX_pop();

/**
 * \brief Inserts a byte into the buffer if not full.
 * Returns if the byte was inserted or not.
 * TRUE  - The data has been inserted into the buffer.
 * FALSE - Buffer is full. Byte has not been inserted.
**/
BOOL bufferX_push(BYTE data);

/**
 * \brief  Returns if the buffer is full or not.
 * TRUE   - Buffer is full.
 * FALSE  - Buffer is not full.
**/
BOOL bufferX_is_full();

/**
 * \brief  Returns if the buffer is empty or not.
 * TRUE   - Buffer is empty.
 * FALSE  - Buffer is not empty.
**/
BOOL bufferX_is_empty();

/**
 * \brief Returns the number of bytes which can be inserted into the buffer.
**/
BYTE bufferX_bytes_left();

/**
 * \brief Removes all the elements in the buffer. Flushes the entire buffer.
**/
BYTE bufferX_flush();

/**
 * \brief Returns the maximum size of the buffer. WARNING:(This is always less than 256 bytes).
**/
BYTE bufferX_max_size();

/**
 * \brief This function directly inserts the data into the current address of the buffer.
 * WARNING: This function will overwrite the data in the current address location.
 * \param BYTE data The data to be inserted into the buffer.
 * Returns if the data has been added in the buffer or not.
 * TRUE   - Data has been added.
 * FALSE  - An error occured. The data has not been added to the buffer.
**/
BOOL bufferX_insert_byte(BYTE data);

/**
 * \brief Returns the data at the current address pointed to, by the buffer without
 * changing the address pointer. The data will still exist in the buffer.
 * \param offset The number of locations the address pointer needs to be decremented in order
 * to get access to the data.
**/
BYTE bufferX_peek(BYTE offset);

/**
 * \brief Increments the address which is being pointed to by the buffer.
 * WARNING: This function will cause the address to be incremented by 1.
 * This will cause you to skip over data if the current address holds data which 
 * you would like to access.
 * Returns if the address has been incremented or not.
 * TRUE   - The address has been incremented.
 * FALSE  - An error occured. The address could not be incremented.
**/
BOOL bufferX_inc();

/**
 * \brief Decrements the address which is being pointed to by the buffer.
 * WARNING: This function will cause the address to be decremented by 1.
 * This will cause you to skip over data if the current address holds data which 
 * you would like to access.(This function is equivalent to a pop without returning
 * the data which has been removed).
 * Returns if the address has been decremented or not.
 * TRUE   - The address has been decremented.
 * FALSE  - An error occured. The address could not be decremented.
**/
BOOL bufferX_dec();

/**
 * \brief  Copies the source bytes into the destination bytes.
 * \param src The source address location.
 * \param dst The destination address into which the data has been copied.
 * \param size The number of bytes which need to be copied from the source address
 * into the destination address.
**/
BYTE bufferX_copy_bytes(__xdata BYTE *src,__xdata BYTE *dst,BYTE size);

#endif
