- git clone https://github.com/RacingTornado/fx2lib
- cd fx2lib
- make
- For fast_uart , git checkout fast_uart
- make
- cd examples/fast_uart
- make
- ./download.sh build/uart_main.ihx

By default a fast_uart(0x30) is inserted in the main while loop. You need to open up a console, I use

sudo minicom -H -w fastuart

You should have the parameters of fastuart set to 115200. You should see the values on the terminal.

The PA2 pin must be connected to the rx pin on the serial adaptor.


I2C:

- Load the program 
- PA6 is SCL , PA7 is SDA. Connect these to your device. 
- For TX load the write address and data to be written.
write_addr[0] = 0xa0;
write_addr[1] = 0x00;
write_addr[2] = 0x00;
write_data[0] = 0x24;
//Address, data, address length and data length
I2CPutTX(&write_addr[0],&write_data[0],0x02,0x01);
- Call i2c_control() in your main loop.
- You should see the I2C lines toggle.
	
