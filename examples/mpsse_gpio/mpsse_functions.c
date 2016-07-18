#include <fx2regs.h>
#include <fx2macros.h>
#include "ftdi_conf.h"
#include "stdio.h"
#include "mpsse_utils.h"

/* '3.3.1 Clock Data Bytes Out on +ve clock edge MSB first (no read)';
*/
void func_CLOCK_BYTES_OUT_POS_MSB()
{
    printf("3.3.1 Clock Data Bytes Out on +ve clock edge MSB first (no read);\r\n");
    clock_obyte_data_pos(0);

}
/* '3.3.2 Clock Data Bytes Out on -ve clock edge MSB first (no read)';
*/
void func_CLOCK_BYTES_OUT_NEG_MSB()
{
    printf("3.3.2 Clock Data Bytes Out on -ve clock edge MSB first (no read);\r\n");
    clock_obyte_data_neg(0);
}
/* '3.3.3 Clock Data Bits Out on +ve clock edge MSB first (no read)';
*/
void func_CLOCK_BITS_OUT_POS_MSB()
{
    printf("3.3.3 Clock Data Bits Out on +ve clock edge MSB first (no read);\r\n");
    set_breakpoint_2();
    //clock_obits_data_pos(0);

}
/* '3.3.4 Clock Data Bits Out on -ve clock edge MSB first (no read)';
*/
void func_CLOCK_BITS_OUT_NEG_MSB()
{
    printf("3.3.4 Clock Data Bits Out on -ve clock edge MSB first (no read);\r\n");
    //clock_obits_data_neg(0);
    set_breakpoint_2();
}
/* '3.3.5 Clock Data Bytes In on +ve clock edge MSB first (no write)';
*/
void func_CLOCK_BYTES_IN_POS_MSB()
{
    printf("3.3.5 Clock Data Bytes In on +ve clock edge MSB first (no write);\r\n");
    //clock_ibyte_data_pos(0);
        set_breakpoint_2();

}
/* '3.3.6 Clock Data Bytes In on -ve clock edge MSB first (no write)';
*/
void func_CLOCK_BYTES_IN_NEG_MSB()
{
    printf("3.3.6 Clock Data Bytes In on -ve clock edge MSB first (no write);\r\n");
    //clock_ibyte_data_neg(0);
        set_breakpoint_2();

}
/* '3.3.7 Clock Data Bits In on +ve clock edge MSB first (no write)';
*/
void func_CLOCK_BITS_IN_POS_MSB()
{
    printf("3.3.7 Clock Data Bits In on +ve clock edge MSB first (no write);\r\n");
        set_breakpoint_2();
    //clock_ibits_data_pos(0);
}
/* '3.3.8 Clock Data Bits In on -ve clock edge MSB first (no write)';
*/
void func_CLOCK_BITS_IN_NEG_MSB()
{
    printf("3.3.8 Clock Data Bits In on -ve clock edge MSB first (no write);\r\n");
        set_breakpoint_2();
    //clock_ibits_data_neg(0);
}
/* '3.3.9 Clock Data Bytes In and Out MSB first,edged';
*/
void func_CLOCK_BYTES_IN_OUT_NORMAL_MSB()
{
    printf("3.3.9 Clock Data Bytes In and Out MSB first,edged;\r\n");
        set_breakpoint_2();

    //clock_iobyte_data(0,0);
}
/* '3.3.9 Clock Data Bytes In and Out MSB first,edged';
*/
void func_CLOCK_BYTES_IN_OUT_INVERTED_MSB()
{
    printf("3.3.9 Clock Data Bytes In and Out MSB first,edged;\r\n");
        set_breakpoint_2();

    //clock_iobyte_data(1,0);
}
/* '3.3.10 Clock Data bits In and Out MSB first';
*/
void func_CLOCK_BITS_IN_OUT_NORMAL_MSB()
{
    printf("3.3.10 Clock Data bits In and Out MSB first;\r\n");
        set_breakpoint_2();

    //clock_iobits_data(0,0);
}
/* '3.3.10 Clock Data bits In and Out MSB first,edged';
*/
void func_CLOCK_BITS_IN_OUT_INVERTED_MSB()
{
    printf("3.3.10 Clock Data bits In and Out MSB first,edged;\r\n");
        set_breakpoint_2();

    //clock_iobits_data(1,0);
}
/* '3.4.1 Clock Data Bytes Out on +ve clock edge LSB first (no read)';
*/
void func_CLOCK_BYTES_OUT_POS_LSB()
{
    printf("3.4.1 Clock Data Bytes Out on +ve clock edge LSB first (no read);\r\n");
        set_breakpoint_2();

    //clock_obyte_data_pos(1);
}
/* '3.4.2 Clock Data Bytes Out on -ve clock edge LSB first (no read)';
*/
void func_CLOCK_BYTES_OUT_NEG_LSB()
{
    printf("3.4.2 Clock Data Bytes Out on -ve clock edge LSB first (no read);\r\n");
    set_break_point();
    clock_obyte_data_neg(1);
}
/* '3.4.3 Clock Data Bits Out on +ve clock edge LSB first (no read)';
*/
void func_CLOCK_BITS_OUT_POS_LSB()
{
    printf("3.4.3 Clock Data Bits Out on +ve clock edge LSB first (no read);\r\n");
    //0x19
    set_break_point();
    clock_bits_out_jtag();
}
/* '3.4.4 Clock Data Bits Out on -ve clock edge LSB first (no read)';
*/
void func_CLOCK_BITS_OUT_NEG_LSB()
{
    printf("3.4.4 Clock Data Bits Out on -ve clock edge LSB first (no read);\r\n");
    //clock_obits_data_neg(1);
    //0x1b
    set_break_point();
    clock_bits_out_jtag();
}
/* '3.4.5 Clock Data Bytes In on +ve clock edge LSB first (no write)';
*/
void func_CLOCK_BYTES_IN_POS_LSB()
{
    printf("3.4.5 Clock Data Bytes In on +ve clock edge LSB first (no write);\r\n");
        set_breakpoint_2();
    //clock_ibyte_data_pos(1);
}
/* '3.4.6 Clock Data Bytes In on -ve clock edge LSB first (no write)';
*/
void func_CLOCK_BYTES_IN_NEG_LSB()
{
    printf("3.4.6 Clock Data Bytes In on -ve clock edge LSB first (no write);\r\n");
            set_breakpoint_2();

    //clock_ibyte_data_neg(1);
}
/* '3.4.7 Clock Data Bits In on +ve clock edge LSB first (no write)';
*/
void func_CLOCK_BITS_IN_POS_LSB()
{
    printf("3.4.7 Clock Data Bits In on +ve clock edge LSB first (no write);\r\n");
        set_breakpoint_2();

    //clock_ibits_data_pos(1);
}
/* '3.4.8 Clock Data Bits In on -ve clock edge LSB first (no write)';
*/
void func_CLOCK_BITS_IN_NEG_LSB()
{
    printf("3.4.8 Clock Data Bits In on -ve clock edge LSB first (no write);\r\n");
        set_breakpoint_2();

    //clock_ibits_data_neg(1);
}
/* '3.4.9 Clock Data Bytes In and Out LSB first,edged';
*/
void func_CLOCK_BYTES_IN_OUT_NORMAL_LSB()
{
    printf("3.4.9 Clock Data Bytes In and Out LSB first,edged;\r\n");
    read_write_bytes_JTAG();
}
/* '3.4.9 Clock Data Bytes In and Out LSB first,edged';
*/
void func_CLOCK_BYTES_IN_OUT_INVERTED_LSB()
{
    printf("3.4.9 Clock Data Bytes In and Out LSB first,edged;\r\n");
    read_write_bytes_JTAG();
}
/* '3.4.10 Clock Data Bits In and Out LSB first,edged';
*/
void func_CLOCK_BITS_IN_OUT_NORMAL_LSB()
{
    printf("3.4.10 Clock Data Bits In and Out LSB first,edged;\r\n");
            set_break_point();
    read_write_bits_JTAG();
}
/* '3.4.10 Clock Data Bits In and Out LSB first,edged';
*/
void func_CLOCK_BITS_IN_OUT_INVERTED_LSB()
{
    printf("3.4.10 Clock Data Bits In and Out LSB first,edged;\r\n");
    read_write_bits_JTAG();
}
/* '3.5.1 Clock Data to TMS pin (no read)';
*/
void func_CLOCK_DATA_TMS_POS()
{
    printf("3.5.1 Clock Data to TMS pin (no read);\r\n");
    clock_bits_tms();
}
/* '3.5.1 Clock Data to TMS pin (no read)';
*/
void func_CLOCK_DATA_TMS_NEG()
{
    //printf("3.5.1 Clock Data to TMS pin (no read);\r\n");
    clock_bits_tms();
}
/* '3.5.2 Clock Data to TMS pin with read';
*/
void func_TMS_READ_POS_POS()
{
    printf("3.5.2 Clock Data to TMS pin with read;\r\n");
            set_break_point();
    read_bits_write_TMS_JTAG();
}
/* '3.5.2 Clock Data to TMS pin with read';
*/
void func_TMS_READ_NEG_POS()
{
    printf("3.5.2 Clock Data to TMS pin with read;\r\n");
    read_bits_write_TMS_JTAG();
}
/* '3.5.2 Clock Data to TMS pin with read';
*/
void func_TMS_READ_POS_NEG()
{
    printf("3.5.2 Clock Data to TMS pin with read;\r\n");
    read_bits_write_TMS_JTAG();
}
/* '3.5.2 Clock Data to TMS pin with read';
*/
void func_TMS_READ_NEG_NEG()
{
    printf("3.5.2 Clock Data to TMS pin with read;\r\n");
    read_bits_write_TMS_JTAG();
}
/* '3.6.1 Set Data bits LowByte';
*/
void func_SET_BITS_LOW()
{
    unsigned char a;
    unsigned char b;
    printf("3.6.1 Set Data bits LowByte;\r\n");
    //Look again and verify that this can actually be done
    a = get_next_byte();
    b = get_next_byte();
    IOA = a;
    OEA = b;
    OEA = b;
    //decrement_total_byte_count(3);
    //printf("Write direction %02x, value %02x length %02d\r\n",a,b, ep2_buffer.total_length);
}
/* '3.6.2 Set Data bits High Byte';
*/
void func_SET_BITS_HIGH()
{
    printf("3.6.2 Set Data bits High Byte;\r\n");
    OEB = get_next_byte();
    IOB = get_next_byte();
    //decrement_total_byte_count(3);
    //printf("Write high bytes\r\n");
}
/* '3.6.3 Read Data bits LowByte';
*/
void func_GET_BITS_LOW()
{
   // printf("3.6.3 Read Data bits LowByte;\r\n");
}
/* '3.6.4 Read Data bits HighByte';
*/
void func_GET_BITS_HIGH()
{
  //  printf("3.6.4 Read Data bits HighByte;\r\n");
}
/* '3.7.1 Connect TDI to TDO for Loopback';
*/
void func_LOOPBACK_START()
{
   // printf("3.7.1 Connect TDI to TDO for Loopback;\r\n");
}
/* '3.7.2 Disconnect TDI to TDO for Loopback';
*/
void func_LOOPBACK_END()
{
   // printf("3.7.2 Disconnect TDI to TDO for Loopback;\r\n");
}
/* '3.8.1 Set TCK/SK Divisor (FT2232D)';
*/
void func_TCK_DIVISOR()
{
   // printf("3.8.1 Set TCK/SK Divisor (FT2232D);\r\n");
}
/* 'Flush buffer';
*/
void func_SEND_IMMEDIATE()
{
    printf("Flush buffer;\r\n");
    flush_ep1in_data();
}
/* '5.2 Wait On I/O High';
*/
void func_WAIT_ON_HIGH()
{
   // printf("5.2 Wait On I/O High;\r\n");
}
/* '5.3 Wait On I/O Low';
*/
void func_WAIT_ON_LOW()
{
   // printf("5.3 Wait On I/O Low;\r\n");
}
/* 'Disables the clk divide by 5 to allow for a 60MHz master clock. ';
*/
void func_DIS_DIV_5()
{
   // printf("Disables the clk divide by 5 to allow for a 60MHz master clock. ;\r\n");
}
/* 'Enables the clk divide by 5 to allow for backward compatibility with FT2232D';
*/
void func_EN_DIV_5()
{
  //  printf("Enables the clk divide by 5 to allow for backward compatibility with FT2232D;\r\n");
}
/* 'Enables 3 phase data clocking. Used by I2C interfaces to allow data on both clock edges.';
*/
void func_EN_3_PHASE()
{
  //  printf("Enables 3 phase data clocking. Used by I2C interfaces to allow data on both clock edges.;\r\n");
}
/* 'Disables 3 phase data clocking.';
*/
void func_DIS_3_PHASE()
{
   // printf("Disables 3 phase data clocking.;\r\n");
}
/* 'Allows for a clock to be output without transferring data. Commonly used in the JTAG state machine. Clocks counted in terms of numbers of bits';
*/
void func_CLK_BITS()
{
   // printf("Allows for a clock to be output without transferring data. Commonly used in the JTAG state machine. Clocks counted in terms of numbers of bits;\r\n");
}
/* 'Allows for a clock to be output without transferring data. Commonly used in the JTAG state machine. Clocks counted in terms of numbers of bytes';
*/
void func_CLK_BYTES()
{
   // printf("Allows for a clock to be output without transferring data. Commonly used in the JTAG state machine. Clocks counted in terms of numbers of bytes;\r\n");
}
/* 'Allows for a clock to be output without transferring datauntil a logic 1 input on GPIOL1stops the clock.';
*/
void func_CLK_WAIT_HIGH()
{
   // printf("Allows for a clock to be output without transferring datauntil a logic 1 input on GPIOL1stops the clock.;\r\n");
}
/* 'Allows for a clock to be output without transferring datauntil a logic 0 input on GPIOL1stops the clock.';
*/
void func_CLK_WAIT_LOW()
{
   // printf("Allows for a clock to be output without transferring datauntil a logic 0 input on GPIOL1stops the clock.;\r\n");
}
/* 'Enable adaptive clocking';
*/
void func_EN_ADAPTIVE()
{
   // printf("Enable adaptive clocking;\r\n");
}
/* 'Disable adaptive clocking';
*/
void func_DIS_ADAPTIVE()
{
    //printf("Disable adaptive clocking;\r\n");
}
/* 'Allows for a clock to be output without transferring data until a logic1 input on GPIOL1stops the clock or a set number of clock pulses are sent. Clocks counted in terms of numbers of bytes';
*/
void func_CLK_BYTES_OR_HIGH()
{
    //printf("Allows for a clock to be output without transferring data until a logic1 input on GPIOL1stops the clock or a set number of clock pulses are sent. Clocks counted in terms of numbers of bytes;\r\n");
}
/* 'Allows for a clock to be output without transferring datauntil a logic 0 input on GPIOL1stops the clock or a set number of clock pulses are sent. Clocks counted in terms of numbers of bytes';
*/
void func_CLK_BYTES_OR_LOW()
{
    //printf("Allows for a clock to be output without transferring datauntil a logic 0 input on GPIOL1stops the clock or a set number of clock pulses are sent. Clocks counted in terms of numbers of bytes;\r\n");
}
