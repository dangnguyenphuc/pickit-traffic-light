#include "uart.h"
#include "..\timer\timer.h"
#include "..\lcd\lcd.h"

#define INIT_RECEIVE    0
#define DISPLAY         1

#define WAIT_CMD    0
#define WAIT_BYTE2  1
#define WAIT_DATA   2

unsigned char dataReceive;
unsigned char statusReceive = INIT_RECEIVE;
unsigned char statusUartReceive = WAIT_CMD;
unsigned char ColumnDislayMoney = 0;
unsigned char dataFromUart[8];
unsigned char kindOfData;
void init_uart()
{

	SPBRGH = 0;  //Baud Rate Generator Register 
	//Baud Rate =  FOSC/(64 ([SPBRGH:SPBRG] + 1)) 
	//when SYNC = 0, BRG16 = 1, BRGH = 0 => Baud Rate = F/[16(SPBRG + 1)]
        // => SPBRG = F/(16*Baud Rate) - 1
	SPBRG = 129; //9600
	TXSTAbits.TXEN = 1; //Transmit enabled
	TXSTAbits.BRGH = 0; //Low Speed
	BAUDCONbits.BRG16 = 1; //16-bit Baud Rate Generator

	TXSTAbits.SYNC = 0; //Asynchronous mode
	RCSTAbits.SPEN = 1; //Enables
	RCSTAbits.CREN = 1; //Enables receive

	PIR1bits.RCIF = 0;  // clear interrupt flag uart
	PIE1bits.RCIE = 1;	//enable interrupt uart
	IPR1bits.RCIP = 0;	// Priority: low
	TRISCbits.RC6 = 0; // RC6 output
	TRISCbits.RC7 = 1; // RC7 input
}
void uart_delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}
void uart_putchar(unsigned char data)
{
	while(PIR1bits.TXIF == 0);
        TXREG = data;
	PIR1bits.TXIF = 0;
}

void uart_send_str(const char *str)
{
	while(*str)
	{
		uart_putchar(*str);
		*str++;
	}
}

void UartSendString(const rom char *str)
{
	while(*str)
	{
		uart_putchar(*str);
		*str++;
	}
}

void uart_isr()
{
    unsigned char tempReceive;
    tempReceive = RCREG;

    switch (statusUartReceive)
    {
    case WAIT_CMD:
        if (tempReceive == 0x04)
            statusUartReceive = WAIT_BYTE2;
        break;
    case WAIT_BYTE2:
        //if (tempReceive == 0x01)
        //LcdClearS();
        kindOfData = tempReceive;
        statusUartReceive = WAIT_DATA;
        break;
    case WAIT_DATA:
        //LcdPrintNumS(1,0,tempReceive);
        if (kindOfData>=0 && kindOfData<=7)
            dataFromUart[kindOfData] = tempReceive;
        statusUartReceive = WAIT_CMD;
        break;
    }
}

