/** \file include/i2c/api.h
 * This file is for defining a common API for accessing I2C.
 **/

#ifndef I2C_API_H
#define I2C_API_H

#include "fx2types.h"
#include "stdarg.h"

/**
 * \brief Initalizes I2C.
 * Returns TRUE if initialization is successful.
 **/
BOOL i2cX_init(enum i2c_speed speed, ...);

/**
 * enum Standard available i2c speeds.
 *
**/
enum i2c_speed {
    HIGH_SPEED = -2,
    FULL_SPEED = -1,
    SPEED_INVALID = 0,
    SPEED_FASTEST = 1,
    SPEED_FX2     = 2
};

/**
 * \brief Sets the i2c_speed to one of the allowed parameters.
 *   Possible I2C speeds:
 *    \li HIGH_SPEED	(not supported)
 *    \li FULL_SPEED	(not supported)
 *    \li SPEED_INVALID
 *    \li SPEED_FASTEST
 *    \li SPEED_FX2	(currently supported)
 * Returns TRUE if successful.
**/
BOOL i2cX_set_speed(enum i2c_speed speed);

/**
 * \brief Returns the i2c_speed currently being used.
**/
enum i2c_speed i2cX_get_speed();

/**
 * \brief Transmits data through I2C.
 * \param c The character to be sent out.
**/

void i2cX_tx(char c);

/**
 * \brief Returns if the transmit is blocking or not.
 * FALSE - Non Blocking
 * TRUE - Blocking
**/

BOOL i2cX_tx_will_block();

/**
 * \brief Returns how many more bytes can be loaded into the buffer.
**/
BYTE i2cX_tx_queue_len();

/**
 * \brief Receives data through I2C.
 * Returns one byte at a time from the queue.
 *
**/
char i2cX_rx();

/**
 * \brief Returns if the receive is blocking or not.
 * FALSE - Non Blocking
 * TRUE - Blocking
**/
BOOL i2cX_rx_will_block();

/**
 * \brief Returns count number of bytes present in the buffer.
 *
**/
BYTE i2cX_rx_queue_len();

#endif
