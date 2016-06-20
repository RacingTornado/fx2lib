/*
I2C Queue for reading and writing data;;
*/


#define I2C_ELEMENTS 5
#define I2C_DATA 5
#define I2C_SIZE (I2C_ELEMENTS + 1)
struct i2c_client {
//Address
unsigned short addr;
//Upto 5 data bytes can be written
unsigned char data[5];
//The length of data to write to I2C lines
//Does not include the address byte
unsigned char length;
 };

__xdata struct i2c_client i2c_queue[I2C_SIZE];
__xdata unsigned char I2CIn, I2COut;
unsigned char addr;
unsigned char data[I2C_DATA];
unsigned char length;
unsigned char retries;
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

void I2CInit(void)
{
    I2CIn = I2COut = 0;
}

__bit I2CPutTX(unsigned char addr, unsigned char * data)
{
    unsigned char i;
    if(I2CIn == (( I2COut - 1 + I2C_SIZE) % I2C_SIZE))
    {
        return 1; /* Queue Full*/
    }

    i2c_queue[I2CIn].addr = addr;

    for(i=0; data[i]!='\0'; ++i)
    {
	i2c_queue[I2CIn].data[i]= data[i];
    }
    //Address length
    i2c_queue[I2CIn].length = i;
    I2CIn = (I2CIn + 1) % I2C_SIZE;

    return 0; // No errors
}

//This function loads the value of address and 5 data bytes and returns
//This value is then used by the main program
__bit I2CGetTX()
{
    unsigned char i;
    if((I2CIn == I2COut))
    {
        return 1; /* Queue Empty - nothing to get*/
    }
    //Load address
    addr =  i2c_queue[I2COut].addr;
    for(i=0; i < i2c_queue[I2COut].length ; ++i)
    {
        data[i] = i2c_queue[I2CIn].data[i];
    }
    length = i2c_queue[I2CIn].length;
    retries = 3;
    I2COut = (I2COut + 1) % I2C_SIZE;
    return 0; // No errors
}


__bit I2CCheckTX()
{

    return 0;
//    if((I2CIn == I2COut))
//    {
//        return 1; /* Queue Empty - nothing to get*/
//    }
//
//    return 0; // No errors
}
