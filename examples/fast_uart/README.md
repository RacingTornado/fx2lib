
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


