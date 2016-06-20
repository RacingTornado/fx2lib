#ifndef I2C_UTILS_
#define I2C_UTILS_

#define I2C_ELEMENTS 5
#define I2C_DATA 5
#define I2C_SIZE (I2C_ELEMENTS + 1)
struct i2c_client {
//Address
unsigned short addr[3];
//Upto 5 data bytes can be written
unsigned char data[5];
//The length of data to write to I2C lines
//Does not include the address byte
unsigned char data_length;
unsigned char addr_length;
 };



void I2CInit(void);
__bit I2CPutTX(unsigned char * addr, unsigned char * data, unsigned char addr_length, unsigned char data_length);
__bit I2CPutRX(unsigned char addr, unsigned char addr_length ,unsigned char data_length);
__bit I2CGetTX();
__bit I2CCheckTX();

//Variable declarations for address and data
extern unsigned char addr[I2C_ADDR];
extern unsigned char data[I2C_DATA];
extern unsigned char length;
extern unsigned char retries;

#endif
