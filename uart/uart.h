#ifndef _UART_H_
#define _UART_H_

#include <p18f4620.h>

extern unsigned char dataUartReceive;
extern unsigned char dataFromUart[8];
extern unsigned char kindOfData;
void init_uart();
void uart_putchar(unsigned char data);
void uart_send_str(const char *str);
void UartSendString(const rom char *str);
void uart_isr();
void UartSendDataGraph(int value_1,int value_2,int value_3,int value_4);

void UartSendNumToString(long num);
unsigned long uart_power_of(int A, int x);
#endif