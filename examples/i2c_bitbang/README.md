I2C:

- git clone repo
- cd fx2lib
- git submodule init
- git submodule update
- Make sure FXLOAD is already installed on Ubuntu
- make in the fx2lib
- cd examples/i2c_bitbang
- Load the program 
- PA6 is SCL , PA7 is SDA. Connect these to your device. 
- For TX load the write address and data to be written.
- Call i2c_control() in your main loop.
- You should see the I2C lines toggle.

I2C Write Commands:

write_addr[0] = 0xa0;
write_addr[1] = 0x00;
write_addr[2] = wr_addr;
write_data[0] = 0x24;
//Address, data, address length and data length
I2CPutTX(&write_addr[0],&write_data[0],0x03,0x01);

- The I2C lib is programmable. The number of bytesto write as address is loaded into the 3rd parameter of I2CPutTX().
- write_addr holds the data which is copied into the i2c_client queue.
- The length data to be written is the 4th parameter if the I2CPutTX()
- The data which is to be written is loaded into the write_data buffer. 
__bit I2CPutTX(unsigned char * addr, unsigned char * data, unsigned char addr_length, unsigned char data_length)
- The first param is pointer to the address, second a pointer to the data, the 3rd is the address length, the 4th is the data length.

I2C Read Data:

- The read data logic uses 2 queues. One for the controller
- the second queue is to hold the data which has been read.
- To use the i2c_read command 
- We need to first call I2CPutRXRead
- I2CPutRXRead(&write_addr[0],0x01,0x01);
- __bit I2CPutRXRead(unsigned char * addr, unsigned char addr_length, unsigned char data_length)
- The first parameter is the pointer to the address. This should end with a 1(atleast the first byte) for a read operation. The second parameter is the length of the address and the third parameter gives the length of the data to be read from the I2C. The I2C read operation is highle dependent on the device which is being used. For the EEPROM, it appears that we need to NAK after the nth byte for reading n bytes. Since hardcoding such instructions would not be appropriate, the current I2C control, only read one byte and NAK's it. The data_length should always be 1 for the EEPROM. This can be customized in the line number 457 of ic_utils.c in the controller. 
- Once the I2C completes its read operation, it will put the data into a second queue. To read from this queue we need to explicitly declare these 2 variables:
- __xdata unsigned char write_addr[I2C_ADDR];
- __xdata unsigned char write_data[I2C_DATA];
- These 2 variables have to declared in the main with the same name since the i2c_utils will look for this. 
- __bit I2CGetRXData()
- A call to that function loads up the variables so that the data and address from the queue is copied into the write_addr and write_data 
variables.
- A call to i2c_init(3) with number of retries is also needed before using this library.
	
