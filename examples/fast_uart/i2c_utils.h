#ifndef I2C_UTILS_
#define I2C_UTILS_

#include "fx2regs.h"
#include "fx2types.h"

#define I2C_ELEMENTS 5
#define I2C_DATA 5
#define I2C_ADDR 5
#define I2C_SIZE (I2C_ELEMENTS + 1)
#define SDA PA6
#define SCL PA7
#define SYNCDELAY SYNCDELAY4


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




__bit I2CPutTX(unsigned char * addr, unsigned char * data, unsigned char addr_length, unsigned char data_length);
__bit I2CPutRX(unsigned char addr, unsigned char addr_length ,unsigned char data_length);
__bit I2CGetTX();
__bit I2CCheckTX();
void i2c_init(unsigned char retry);
void i2c_control();
void configure_start_timer();
extern void fast_uart(unsigned char a, unsigned char b);



/*************************
I2C Controller
*************************/

enum isr_state
        {
        state_tx=0, //0
        state_rx=1 ,  //1
        state_wait=2
        };
enum i2c_states
        {
idle = 0,
start = 1,
address = 2,
addr_ack = 3,
data_write = 4,
data_write_ack = 5,
data_read = 6,
data_read_ack = 7,
stop = 8,
read_addr_ack = 9,
read_data_ack = 10,
        };
extern enum isr_state tx_rx;
extern unsigned char tx_i2c_buffer;
extern unsigned char rx_i2c_buffer;
extern unsigned char bit_count;
extern unsigned char tx_i2c_buffer;
extern unsigned char bit_count;
//Variable declarations for address and data
extern unsigned char addr[I2C_ADDR];
extern unsigned char data[I2C_DATA];
extern unsigned char length;
extern unsigned char retries;

#endif
