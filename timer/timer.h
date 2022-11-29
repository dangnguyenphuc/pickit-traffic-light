#ifndef _TIMER_H_
#define _TIMER_H_

#include <p18f4620.h>

void init_timer0(unsigned int count);
void start_timer0(void);
void stop_timer0(void);
void init_timer1(unsigned int count);
void start_timer1(void);
void stop_timer1(void);
void init_timer2(unsigned char count);
void start_timer2(void);
void stop_timer2(void);
void init_timer3(unsigned int count);
void start_timer3(void);
void stop_timer3(void);

void timer0_isr(void);
void timer1_isr(void);
void timer2_isr(void);
// xay dung ham cap cao cho timer, nhung ham se duoc lop user goi
void SetTimer0_ms(int time);
void SetTimer1_ms(int time);
void SetTimer3_ms(int time);
void v_timer0(unsigned int ms);
void v_timer1(unsigned int ms);


extern unsigned char flag_timer0,flag_timer1,flag_timer2,flag_timer3;
// extern la khai bao bien toan cuc chuong trinh, khong duoc gan gia tri

#endif