#ifndef _UART_H_
#define _UART_H_

#include <p18f4620.h>

extern unsigned char statusReceive;
extern unsigned char flagOfDataReceiveComplete;
extern unsigned char indexOfDataReceive;
extern unsigned char numberOfDataReceive;
extern unsigned char dataReceive [50];
extern unsigned char indexOfdataSend;
extern unsigned char numberOfDataSend;
extern unsigned char dataSend [50];

void init_uart();
void uart_putchar(unsigned char data);
void uart_send_str(const char *str);
void UartSendString(const rom char *str);
void uart_isr();
void UartSendDataGraph(int value_1,int value_2,int value_3,int value_4);
void uart_isr_simulate_machine();

void UartSendNumToString(long num);
unsigned long uart_power_of(int A, int x);
#endif