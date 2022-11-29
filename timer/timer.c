#include "timer.h"

unsigned int timer0_value = 0,timer1_value = 0,timer2_value = 0,timer3_value = 0;
unsigned char flag_timer0 = 0,flag_timer1 = 0,flag_timer2 = 0,flag_timer3 = 0;
unsigned int v_cnt0 = 0,v_cnt1 = 0;
unsigned int timer0_cnt = 0,timer1_cnt = 0,timer2_cnt = 0,timer3_cnt = 0;
unsigned int time0_MUL = 1,time1_MUL = 1,time2_MUL = 1,time3_MUL = 1;

//--------------------TIMER0-------------------------
void init_timer0(unsigned int count)
{
	T0CONbits.TMR0ON = 0;			// turn off timer0
	timer0_value = count;
	T0CONbits.T08BIT = 0;			// set mode 16bit timer0
	TMR0L = (65535 - timer0_value)%256;	// count timer0_value times
	TMR0H = (65535 - timer0_value)/256;
	T0CONbits.T0CS = 0;                     // select internal clock include crystal
	T0CONbits.PSA = 1;			// no prescaler
	INTCONbits.T0IE = 1;			// enable timer0 interrupt
	INTCONbits.T0IF = 0;			// clear interrupt timer0 flag
	INTCON2bits.TMR0IP = 1;			// timer0 interrupt high priority 	
	T0CONbits.TMR0ON = 1;                   // start timer 0
}

void stop_timer0(void)
{
	T0CONbits.TMR0ON = 0;	
}

void start_timer0(void)
{
	
	TMR0L = (65535 - timer0_value)%256;		// count 50000 times
	TMR0H = (65535 - timer0_value)/256;
	T0CONbits.TMR0ON = 1;
}

//--------------------TIMER1-------------------------
void init_timer1(unsigned int count)
{
	T1CONbits.TMR1ON = 0;			// turn off timer 1
	timer1_value = count;
	TMR1H = ((65535- timer1_value)/256);	// count timer1_value times
	TMR1L = ((65535- timer1_value)%256);	//
	PIE1bits.TMR1IE = 1;			// enable timer1 interrupt
	PIR1bits.TMR1IF = 0;			// clear timer1 interrupt flag
	IPR1bits.TMR1IP = 0;			// timer1 interrupt low priority 	
	//T1CON
	T1CONbits.RD16 = 1;			// select timer1 16 bits mode
	T1CONbits.T1RUN = 1;			// 
	T1CONbits.T1CKPS0 = 0;			//
	T1CONbits.T1CKPS1 = 0;			// prescaler 1:1
	T1CONbits.T1OSCEN = 0;  		// oscillator invert and feedback for external oscillator
//	T1CONbits.T1SYNC = 0;			// this bit ignore when TMR1CS = 0
	T1CONbits.TMR1CS = 0;			// select internal clock source
	T1CONbits.TMR1ON = 1;			// start timer 1
}

void start_timer1(void)
{
	TMR1H = ((65535-timer1_value)/256);
	TMR1L = ((65535-timer1_value)%256);
	T1CONbits.TMR1ON = 1;
}

void stop_timer1(void)
{
	T1CONbits.TMR1ON = 0;
}

//--------------------TIMER2-------------------------
void init_timer2(unsigned char count)
{
	T2CONbits.TMR2ON = 0;		// turn off timer2
	TMR2 = 0x00;
	timer2_value = count;           // count timer2_value times
	PR2 = timer2_value;
	PIE1bits.TMR2IE = 1;		// enable timer2 interrupt
	PIR1bits.TMR2IF = 0;		// clear interrupt timer2 flag

	IPR1bits.TMR2IP = 0;		// timer2 interrupt low priority	

	T2CONbits.T2CKPS0 = 0;		//
	T2CONbits.T2CKPS1 = 0;		// prescaler 1:1
	T2CONbits.TMR2ON = 1;		// start timer2
}

void start_timer2(void)
{
	PR2 = timer2_value;
	T2CONbits.TMR2ON = 1;
}

void stop_timer2(void)
{
	T2CONbits.TMR2ON = 0;
}

//--------------------TIMER3-------------------------
void init_timer3(unsigned int count)
{	
	T3CONbits.TMR3ON = 0;			// turn off timer 3
	timer3_value = count;
	TMR3H = ((65535-timer3_value)/256);	// count timer3_value times
	TMR3L = ((65535-timer3_value)%256);	//
	PIE2bits.TMR3IE = 1;			// enable timer3 interrupt
	PIR2bits.TMR3IF = 0;			// clear timer3 interrupt flag

	IPR2bits.TMR3IP = 0;			// timer3 interrupt low priority 	

	//T1CON
	T3CONbits.RD16 = 1;				// select timer3 16 bits mode
	T3CONbits.T3CKPS0 = 0;			//
	T3CONbits.T3CKPS1 = 0;			// prescaler 1:1
//	T3CONbits.T3SYNC = 0;			// this bit ignore when TMR3CS = 0
	T3CONbits.TMR3CS = 0;			// select internal clock source
	T3CONbits.TMR3ON = 1;			// start timer3
}

void start_timer3(void)
{
	TMR3L = (65535 - timer3_value)%256;
	TMR3H = (65535 - timer3_value)/256;
	T3CONbits.TMR3ON = 1;
}

void stop_timer3(void)
{
	T3CONbits.TMR3ON = 0;
}
//Cac ham phuc vu interupt timer
void timer0_isr(void)
{
	timer0_cnt ++;
	if (timer0_cnt >= time0_MUL)
	{
		timer0_cnt = 0;
		flag_timer0 = 1;
                // Noi goi nhung ham doi hoi toc do nhanh va uu tien cao
	}
}

void timer1_isr(void)
{
	timer1_cnt ++;
	if (timer1_cnt >= time1_MUL)
	{
		timer1_cnt = 0;
		flag_timer1 = 1;
	}
        // Noi goi nhung ham doi hoi toc do nhanh va uu tien cao hon ham main
}

void timer2_isr(void)
{
	timer2_cnt ++;
	if (timer2_cnt >= time2_MUL)
	{
		timer2_cnt = 0;
		flag_timer2 = 1;
	}
}

void timer3_isr(void)
{
	timer3_cnt ++;
	if (timer3_cnt >= time3_MUL)
	{
		timer3_cnt = 0;
		flag_timer3 = 1;
	}
}

void SetTimer0_ms(int time)
{
	time0_MUL = time;
	timer0_cnt = 0;
	flag_timer0 = 0;
}

void SetTimer1_ms(int time)
{
	time1_MUL = time/5;
	timer1_cnt = 0;
	flag_timer1 = 0;
}

void SetTimer3_ms(int time)
{
	time3_MUL = time/10;
	timer3_cnt = 0;
	flag_timer3 = 0;
}

//------------------VIRTUAL TIMER----------------------
