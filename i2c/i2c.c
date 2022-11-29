#include "i2c.h"
#include "..\lcd\lcd.h"
#include "..\timer\timer.h"
#include "..\interrupt\interrupt.h"

unsigned char flag_read = WRITE,byte_num = 0,index_add = 0;
unsigned char write_data[11] = {0xd0,0x00,0x22,0x22,0x22,0x07,0x25,0x09,0x10,0x00,0x01};
unsigned char read_data[9] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
unsigned char ack_bit;
unsigned char receive_flag = 0,ack_flag = 0,receive_data = 0;
unsigned char i2c_data = 0;


#define 	Fosc	20000000
#define 	baud	100000
#define         device_control_code  0b1101000

unsigned char BCD2Dec(unsigned char BCD){
	unsigned char L, H;
	L=BCD & 0x0F;
	H=(BCD>>4)*10;
	return (H+L);
}
unsigned char Dec2BCD(unsigned char Dec){
	unsigned char L, H;
	L=Dec % 10;
	H=(Dec/10)<<4;
	return (H+L);
}

void init_i2c(void)
{
	TRIS_SCL = 1;
	TRIS_SDA = 1;

	SSPSTATbits.SMP = 0;
	SSPSTATbits.CKE = 0;

	SSPCON1bits.SSPM0 = 0;			// I2C master mode, Baudrate = Fosc/(4*(SSPADD + 1))
	SSPCON1bits.SSPM1 = 0;			//
	SSPCON1bits.SSPM2 = 0;			//
	SSPCON1bits.SSPM3 = 1;			//

	SSPADD = 49;						// Baudrate = 100kHz
	SSPCON1bits.SSPEN = 1;
}

void i2c_control(unsigned char mRW_bit)
{
    PIR1bits.SSPIF=0;          // clear SSP interrupt bit
    SSPBUF = (device_control_code << 1) + mRW_bit;  // send the control byte
    while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
}

void i2c_start(void)
{
	PIR1bits.SSPIF=0;          // clear SSP interrupt bit
        SSPCON2bits.SEN=1;          // send start bit
        while(!PIR1bits.SSPIF);    // Wait for the SSPIF bit to go back high before we load the data buffer
}


void i2c_stop(void)
{
	PIR1bits.SSPIF=0;          // clear SSP interrupt bit
        SSPCON2bits.PEN=1;          // send stop bit
        while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
}

void i2c_write(unsigned char abyte)
{
	PIR1bits.SSPIF=0;          // clear SSP interrupt bit
        SSPBUF = abyte;              // send databyte
        while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
}

unsigned char i2c_read(void)
{
        PIR1bits.SSPIF=0;          // clear SSP interrupt bit
        SSPCON2bits.RCEN=1;         // set the receive enable bit to initiate a read of 8 bits from the serial eeprom
        while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
        return (SSPBUF);            // Data from eeprom is now in the SSPBUF so return that value
}

void i2c_ack(void)
{
   PIR1bits.SSPIF=0;          // clear SSP interrupt bit
   SSPCON2bits.ACKDT=0;        // clear the Acknowledge Data Bit - this means we are sending an Acknowledge or 'ACK'
   SSPCON2bits.ACKEN=1;        // set the ACK enable bit to initiate transmission of the ACK bit to the serial eeprom
   while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
}

void i2c_nak(void)
{
    PIR1bits.SSPIF=0;           // clear SSP interrupt bit
    SSPCON2bits.ACKDT=1;        // set the Acknowledge Data Bit- this means we are sending a No-Ack or 'NAK'
    SSPCON2bits.ACKEN=1;        // set the ACK enable bit to initiate transmission of the ACK bit to the serial eeprom
    while(!PIR1bits.SSPIF);    // Wait for interrupt flag to go high indicating transmission is complete
}

void write_ds1307(unsigned char address, unsigned char data)
{
	i2c_start();
        i2c_control(0);
        ack_bit = 1;
        while(ack_bit)
            ack_bit = SSPCON2bits.ACKSTAT;
        i2c_write(address);
        ack_bit = 1;
        while(ack_bit)
            ack_bit = SSPCON2bits.ACKSTAT;
        i2c_write(Dec2BCD(data));
        ack_bit = 1;
        while(ack_bit)
            ack_bit = SSPCON2bits.ACKSTAT;
        i2c_stop();
}

unsigned char read_ds1307(unsigned char address)
{
	unsigned char data;
	i2c_start();
        i2c_control(0);
        ack_bit = 1;
        while(ack_bit)
            ack_bit = SSPCON2bits.ACKSTAT;
        i2c_write(address);  
        ack_bit = 1;
        while(ack_bit)
            ack_bit = SSPCON2bits.ACKSTAT;

        i2c_start();     i2c_control(1);
        ack_bit = 1;
        while(ack_bit)
            ack_bit = SSPCON2bits.ACKSTAT;
        data = i2c_read();
        data = BCD2Dec(data);
	return data;
}
