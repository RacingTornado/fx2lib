/*
I2C Queue for reading and writing data;;
*/
#include "i2c_utils.h"
#include <delay.h>


//An I2C client structure. 5 data bytes, 5 address bytes
__xdata struct i2c_client i2c_queue[I2C_SIZE];
__xdata unsigned char I2CTXIn, I2CTXOut;

//Variables used by the I2C controller
//These variables hold data for the current
//transaction. Once the transaction is complete
//they are inserted back into the queue or the
//queue is cleared , depending on whether it is
//a read or write operation.
unsigned char addr[I2C_ADDR];
unsigned char data[I2C_DATA];
unsigned char data_length;
unsigned char addr_length;
unsigned char retries;
unsigned char tx_i2c_buffer;
unsigned char bit_count;
//Read write bit for I2C
__bit rw;
__bit rw_bit;





/************************************************************
I2C Controller
************************************************************/


enum isr_state tx_rx;
//I2C state variable
enum i2c_state my_i2c_states;
unsigned char tx_i2c_buffer;
unsigned char rx_i2c_buffer;
unsigned char bit_count;
//Temporary counters for EEPROM access
unsigned char i ;
unsigned char j;
unsigned long delay1;




/* Very simple queue
 * These are FIFO queues which discard the new data when full.
 * Snippet taken from StackOverflow
 * Queue is empty when in == out.
 * If in != out, then
 *  - items are placed into in before incrementing in
 *  - items are removed from out before incrementing out
 * Queue is full when in == (out-1 + QUEUE_SIZE) % QUEUE_SIZE;
 *
 * The queue will hold QUEUE_ELEMENTS number of items before the
 * calls to QueuePut fail.
 */

void I2CQueueInit(void)
{
    I2CTXIn = I2CTXOut = 0;
    //I2CRXIn = I2CRXOut = 0;


}



 /*******************************************************************************************
    I2C TX Queue - Insert data and address, along with the length. The controller
    uses this data to put the data out on the SDA and SCL lines
 *******************************************************************************************/

//Set up queue structure to write data from
__bit I2CPutTX(unsigned char * addr, unsigned char * data, unsigned char addr_length, unsigned char data_length)
{
    unsigned char i;
    if(I2CTXIn == (( I2CTXOut - 1 + I2C_SIZE) % I2C_SIZE))
    {
        return 1; /* Queue Full*/
    }
    for(i=0; i<addr_length; ++i)
    {
	i2c_queue[I2CTXIn].addr[i]= addr[i];
    }
    i2c_queue[I2CTXIn].addr_length = addr_length;
    i=0;
    for(i=0; i<data_length; ++i)
    {
	i2c_queue[I2CTXIn].data[i]= data[i];
    }
    //Address length
    i2c_queue[I2CTXIn].data_length = data_length;
    I2CTXIn = (I2CTXIn + 1) % I2C_SIZE;

    return 0; // No errors
}

//This function loads the value of address and 5 data bytes and returns
//This value is then used by the main program
__bit I2CGetTX()
{
    unsigned char i;
    if((I2CTXIn == I2CTXOut))
    {
        return 1; /* Queue Empty - nothing to get*/
    }
    //Load address
    for(i=0; i < i2c_queue[I2CTXOut].addr_length ; ++i)
    {
        addr[i] = i2c_queue[I2CTXOut].addr[i];
    }
    for(i=0; i < i2c_queue[I2CTXOut].data_length ; ++i)
    {
        data[i] = i2c_queue[I2CTXOut].data[i];
    }
    data_length = i2c_queue[I2CTXOut].data_length;
    addr_length = i2c_queue[I2CTXOut].addr_length;
    I2CTXOut = (I2CTXOut + 1) % I2C_SIZE;
    return 0; // No errors
}


__bit I2CCheckTX()
{


    if((I2CTXIn == I2CTXOut))
    {
        return 1; /* Queue Empty - nothing to get*/
    }

    return 0; // No errors
}






////Set up queue structure to read the data from
//__bit I2CPutRX(unsigned char * addr, unsigned char addr_length, unsigned char data_length)
//{
//    unsigned char i;
//    if(I2CIn == (( I2COut - 1 + I2C_SIZE) % I2C_SIZE))
//    {
//        return 1; /* Queue Full*/
//    }
//    for(i=0; i<addr_length; ++i)
//    {
//	i2c_queue[I2CIn].addr[i]= addr[i];
//    }
//    i2c_queue[I2CIn].addr_length = i;
//    //Address length
//    i2c_queue[I2CIn].data_length = length;
//    I2CIn = (I2CIn + 1) % I2C_SIZE;
//
//    return 0; // No errors
//}
//
//
////This function returns the data read from I2C into a pointer.
////The caller of this function must provide a pointer to the memory
////location into which the data needs to be read.
//__bit I2CGetRX()
//{
//    unsigned char i;
//    if((I2CIn == I2COut))
//    {
//        return 1; /* Queue Empty - nothing to get*/
//    }
//    //Load address
//    for(i=0; i < i2c_queue[I2COut].addr_length ; ++i)
//    {
//        addr[i] = i2c_queue[I2CIn].addr[i];
//    }
//    for(i=0; i < i2c_queue[I2COut].length ; ++i)
//    {
//        data[i] = i2c_queue[I2CIn].data[i];
//    }
//    data_length = i2c_queue[I2CIn].data_length;
//    addr_length = i2c_queue[I2CIn].addr_length;
//    I2COut = (I2COut + 1) % I2C_SIZE;
//    return 0; // No errors
//}
//
//
//
//
//
//
//
//
//
//
//__bit I2CCheckRX()
//{
//
//    return 0;
////    if((I2CIn == I2COut))
////    {
////        return 1; /* Queue Empty - nothing to get*/
////    }
////
////    return 0; // No errors
//}






void i2c_init(unsigned char retry)
{
    tx_rx = state_wait;
    //Set current state to idle
    my_i2c_states = idle;
    //Initialize queue
    I2CQueueInit();
    //Set OEA register
    OEA |= 0xc0;
    //Initial value for I2C
    SDA = 1;
    SCL = 1;
    rw = 0;
    retries = retry;
    //Keep track of variable number
    i = 0;
}


void configure_start_timer()
{


   TMOD = 0x20;
   SYNCDELAY;
   TR1 = 0;
   SYNCDELAY;
   TH1 = 0xc3;
   SYNCDELAY;
   TL1 = 0x23;
   SYNCDELAY;
   TR1 = 1;
   SYNCDELAY;

}











/*************************************************************************
I2C Controller
*************************************************************************/
void i2c_control()
{
    if ((my_i2c_states == idle) && (tx_rx == state_wait))
    {
        // Look for non empty queue and pull the SDA low keeping SCL high.
        if (I2CCheckTX() == 0)
        {


             if(rw == 0 )
              {
                I2CGetTX();
                fast_uart(data[0],0x04);
              }
              else
              {
                for (delay1 = 0; delay1 <600; delay1 ++)
                {
                __asm
                mov r0, #0xFF
                003$:
                mul ab
                djnz r0,003$
                __endasm;
                }
              }
            my_i2c_states = start;
        }
        else
        {
        }
    }
    else if ((my_i2c_states == start) && (tx_rx == state_wait))
    {
        //Send the start bit
        OEA |= 0xC0;
        PA7 = 0 ;
        //Initial delay between the SDA and SCL lines for start bit
        __asm
        mov r0, #0x03
        001$:djnz r0,001$
        __endasm;
        PA6 = 0;
        my_i2c_states = address;
        //fast_uart(0x55,0x04);

        //I2CGetTX();
    }
    else if ((my_i2c_states == address) && (tx_rx == state_wait))
    {
        //Send the address and check the R/W bit
        tx_i2c_buffer = addr[i];
        bit_count = 0x09;
        tx_rx = state_tx;
        __asm
        mov a, _addr
        rrc a
        mov _rw_bit,c
        __endasm;
        my_i2c_states = read_addr_ack;
        //fast_uart(0x66,0x04);
    }
    else if ((my_i2c_states == read_addr_ack) && (tx_rx == state_wait))
    {
        //Read the ACK bit.
        //OEA &= 0x7f;
        //OEA |= 0x08;
        //PA3 = 1;
        bit_count = 0x02;
        tx_rx = state_rx;
        my_i2c_states = addr_ack;
        //fast_uart(0x77,0x04);
    }
    else if ((my_i2c_states == addr_ack) && (tx_rx == state_wait))
    {
        //Find the number of data bytes to transfer and keep track
        //OEA &= 0xf7;
        //fast_uart(tx_i2c_buffer,0x04);
        if((tx_i2c_buffer & 0x01) == 0x00)
        {


            if(i >= addr_length)
            {
                i = 0;
                if(rw_bit == 1)
                {

                    my_i2c_states = data_read;
                }
                else
                {
                    my_i2c_states = data_write;
                    //fast_uart(0x88,0x04);
                }
            }
            else
            {
                my_i2c_states = address;
                i++;
            }

        }
       else if((tx_i2c_buffer & 0x01) == 0x01)
        {
            if(j >= retries)
            {
                my_i2c_states = stop;
                j = 0;
            }
            else
            {
                my_i2c_states = address;
                j++;
            }
        }
    }
    else if ((my_i2c_states == data_read) && (tx_rx == state_wait))
    {
        //Read the data 8 bit.
        bit_count = 0x09;
        tx_rx = state_rx;
        my_i2c_states = data_read_ack;
    }
    else if ((my_i2c_states == data_read_ack) && (tx_rx == state_wait))
    {
        //Acknowledge the read data always.
        OEA |= 0xC0;
        PA6 = 0 ;
        PA7 = 0 ;
        PA6 = 1 ;
        if(i >= data_length)
        {
            my_i2c_states = stop;
            i = 0;
        }
        else
        {
            my_i2c_states = data_read;
            i++;
        }
    }
    else if ((my_i2c_states == data_write) && (tx_rx == state_wait))
    {
        //Send the 8 bits of data
        //OEA &= 0xf7;
        //OEA |= 0x80;

        tx_i2c_buffer = data[i];
        bit_count = 0x09;
        tx_rx = state_tx;
        my_i2c_states = read_data_ack;
    }
    else if ((my_i2c_states == read_data_ack) && (tx_rx == state_wait))
    {
        //Prepare for reading the data ACK bit.
        bit_count = 0x02;
        tx_rx = state_rx;
        my_i2c_states = data_write_ack;
    }
    else if ((my_i2c_states == data_write_ack) && (tx_rx == state_wait))
    {
        //Read the ACK bit for the written data
        if((tx_i2c_buffer & 0x01) == 0x00)
        {
            if(i >= data_length)
            {
                my_i2c_states = stop;
                i = 0;
                fast_uart(0xAA,0x04);

            }
            else
            {
                my_i2c_states = data_write;
                i++;
            }
        }
        else if((tx_i2c_buffer & 0x01) == 0x01)
        {
            if(j >= retries)
            {
                my_i2c_states = stop;
                j = 0;
            }
            else
            {
                my_i2c_states = data_write;
                j++;
            }
        }

    }
    else if ((my_i2c_states == stop) && (tx_rx == state_wait))
    {
        //Send the stop bit
        OEA |= 0xC0;
        PA6 = 0 ;
        PA7 = 0 ;
        PA6 = 1 ;
        __asm
        mov r0, #0x03
        002$:djnz r0,002$
        __endasm;
        PA7 = 1 ;
        my_i2c_states = idle;
    }

}
