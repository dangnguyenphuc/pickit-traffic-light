#include "main.h"
// Noi khai bao hang so
#define     LED     PORTA
#define     ON      1
#define     OFF     0

//#define     INIT_SYSTEM     0
#define     CALL            1
#define     MESSAGE         2
#define     WAITING         3
// Noi khai bao bien toan cuc
unsigned char arrayMapOfOutput [8] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
unsigned char statusOutput[8] = {0,0,0,0,0,0,0,0};
unsigned char statusSim900 = INIT_SYSTEM;
unsigned char WaitResponse = 0;
// Khai bao cac ham co ban IO
void init_system(void);
void delay_ms(int value);
void OpenOutput(int index);
void CloseOutput(int index);
void TestOutput(void);
void ReverseOutput(int index);
void TestReceiveUart();

unsigned char isButtonCall();
unsigned char isButtonMessage();
void Sim900();
void BaiTap_UART();
unsigned int x_Graph,y_Graph,z_Graph;

#define         GET_DATA        0
#define         SEND_DATA       1

unsigned long dataOfSensor[40];
unsigned long averageSensor =0 ;
unsigned long rawSensor =0 ;
char statusSensor = GET_DATA ;
void GetSensor(void);

char indexOfData = 0;
unsigned long dataOfDistance[20];
unsigned long time=0;
unsigned long averageDistance =0 ;
unsigned long rawDistance =0 ;
char statusDistance = GET_DATA ;
void GetDistance(void);
void delay_trigger(void);
void pulse(void);

void Phase1_GreenOn();
void Phase1_GreenOff();
void Phase1_YellowOn();
void Phase1_YellowOff();
void Phase1_RedOn();
void Phase1_RedOff();
void Phase2_GreenOn();
void Phase2_GreenOff();
void Phase2_YellowOn();
void Phase2_YellowOff();
void Phase2_RedOn();
void Phase2_RedOff();

void fsm_automatic();



////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	int k = 0;
	init_system();
        lcd_clear();
        LcdClearS();
        //TestOutput();
        delay_ms(1000);
//        UartSendString("Xin chao cac ban");
        //uart_putchar(1 + '0');
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            //TestReceiveUart();
            //k = k + 1111;
            //UartSendNumToString(k);
            //UartSendString(" ");
            //scan_key_matrix_with_uart();
            //BaiTap_UART();
//            GetSensor();
            LcdClearS();
            fsm_automatic();
            DisplayLcdScreen();
	}
}
// Hien thuc cac module co ban cua chuong trinh
void delay_ms(int value)
{
	int i,j;
	for(i=0;i<value;i++)
		for(j=0;j<238;j++);
}

void init_system(void)
{
        TRISB = 0x00;
        TRISD = 0x00;		//setup PORTB is output
        TRISA = 0x00;
        init_lcd();
        LED = 0x00;
        
        init_interrupt();
        delay_ms(1000);
        //init_timer0(4695);//dinh thoi 1ms sai so 1%
        //init_timer1(9390);//dinh thoi 2ms
        
        
	init_timer3(46950);//dinh thoi 10ms
//    init_timer3(4695);//dinh thoi 1ms sai so 1%
    
    
	//SetTimer0_ms(2);
        //SetTimer1_ms(10);
	SetTimer3_ms(50); //Chu ky thuc hien viec xu ly input,proccess,output
        init_key_matrix_with_uart();
        init_uart();
        init_adc();
        TRISB = TRISB | 0x03; // RB1, RB0 input
}

void OpenOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED | arrayMapOfOutput[index];
	}

}

void CloseOutput(int index)
{
	if (index >= 0 && index <= 7)
	{
		LED = LED & ~arrayMapOfOutput[index];
	}
}

void ReverseOutput(int index)
{
    if (statusOutput[index]  == ON)
    {
        CloseOutput(index);
        statusOutput[index] = OFF;
    }
    else
    {
        OpenOutput(index);
        statusOutput[index] = ON;
    }
}

void TestOutput(void)
{
	int k;
	for (k=0;k<=7 ;k++ )
	{
		OpenOutput(k);
		delay_ms(500);
		CloseOutput(k);
		delay_ms(500);
	}
}

unsigned char isButtonCall()
{
    if(key_code[0] == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

unsigned char isButtonMessage()
{
    if(key_code[1] == 1)
        return 1;
    else
        return 0;
}

void TestReceiveUart()
{
    LcdPrintStringS(0,0,"   ");
    LcdPrintNumS(0,0,dataUartReceive);
}
void BaiTap_UART()
{
    int i_uart;
    unsigned long pH_value,adc_value;
    adc_value = get_adc_value(0);
    pH_value = (adc_value * 140)/1023;
    uart_putchar(pH_value);
    if (pH_value<100)
    {
        LcdPrintNumS(0,10,pH_value/10);
        LcdPrintStringS(0,11,".");
        LcdPrintNumS(0,12,pH_value%10);
    }
    else
    {
        LcdPrintNumS(0,10,pH_value/10);
        LcdPrintStringS(0,12,".");
        LcdPrintNumS(0,13,pH_value%10);
    }
}

void GetDistance(void)
{
  char i =0;
  switch (statusDistance)
  {
    case GET_DATA :
        time = 0;
        pulse();
        while (PORTBbits.RB1 == 0)
        {
        time++;
        if (time > 1000) break;
        }
        time = 0;
        while (PORTBbits.RB1 == 1)
        {
            time++;
        }
        //time = time*10000/18122;
        rawDistance = time;
        dataOfDistance[indexOfData] = rawDistance;
        indexOfData = (indexOfData + 1)%20;
        statusDistance = SEND_DATA;
        break;
    case SEND_DATA:
        averageDistance = 0;
        for (i=0;i<20;i++)
        {
            averageDistance = averageDistance + dataOfDistance[i];
        }
        averageDistance = averageDistance/20;
        UartSendDataGraph(rawDistance,averageDistance-100,averageDistance/3,0);
        //UartSendNumToString(averageDistance);
        //UartSendString(" ");
        statusDistance = GET_DATA;
        break;
    default:
        statusDistance = GET_DATA;
        break;
  }
}

void GetSensor(void)
{
  char i =0;
  switch (statusSensor)
  {
    case GET_DATA :
        rawSensor = get_adc_value(0);
        LcdPrintStringS(1,0,"    ");
        LcdPrintNumS(1,0,rawSensor);
        dataOfSensor[indexOfData] = rawSensor;
        indexOfData = (indexOfData + 1)%40;
        statusSensor = SEND_DATA;
        //break;
    case SEND_DATA:
        averageSensor = 0;
        for (i=0;i<40;i++)
        {
            averageSensor = averageSensor + dataOfSensor[i];
        }
        averageSensor = averageSensor/40;
        UartSendDataGraph(rawSensor,averageSensor-100,averageSensor/3,0);
        //UartSendNumToString(averageSensor);
        //UartSendString(" ");
        LcdPrintStringS(1,8,"    ");
        LcdPrintNumS(1,8,averageSensor);
        statusSensor = GET_DATA;
        break;
    default:
        statusSensor = GET_DATA;
        break;
  }
}

void delay_trigger(void)
{
    //volatile int i;
    int i;
    for(i = 0; i < 30; i++);
}

void pulse(void)
{
    CloseOutput(1);
    OpenOutput(1);
    delay_trigger();	//delay 10us
    CloseOutput(1);
}

void Phase1_GreenOn()
{
    OpenOutput(0);
}
void Phase1_GreenOff()
{
    CloseOutput(0);
}

void Phase1_YellowOn()
{
    OpenOutput(4);
}
void Phase1_YellowOff()
{
    CloseOutput(4);
}

void Phase1_RedOn()
{
    OpenOutput(6);
}
void Phase1_RedOff()
{
    CloseOutput(6);
}

void Phase2_GreenOn()
{
    OpenOutput(1);
}
void Phase2_GreenOff()
{
    CloseOutput(1);
}

void Phase2_YellowOn()
{
    OpenOutput(5);
}
void Phase2_YellowOff()
{
    CloseOutput(5);
}

void Phase2_RedOn()
{
    OpenOutput(7);
}
void Phase2_RedOff()
{
    CloseOutput(7);
}

void fsm_automatic(){
    cntOfLight = (cntOfLight+1)%20;
    if(cntOfLight==1){
        timeOfLight --;
        timeOfLight_2 --;
    }
    switch(status){
        
        case INIT_SYSTEM:
            
            //TODO:
                // NONE
            
            //Switch
            status = PHASE1_GREEN;
            timeOfLight = green_1_Time;
            timeOfLight_2 = redTime_2;
            break;
            
        case PHASE1_GREEN:
            
            //TODO:
            // Lights up
            Phase1_GreenOn();
            
            Phase1_RedOff();
            Phase1_YellowOff();
            
            
            Phase2_RedOn();
            
            Phase2_GreenOff();
            Phase2_YellowOff();
            
            // Display times:
            // 1st:
            LcdPrintStringS(0,0,"GREEN 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0,"RED 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            
            
            //Switch
            if(timeOfLight <= 0){
                status = PHASE1_YELLOW;
                timeOfLight = yellow_1_Time;
            }
            break;
        
        case PHASE1_YELLOW:
            
            //TODO:
            
            // Light up
            Phase1_YellowOn();
            
            Phase1_GreenOff();
            Phase1_RedOff();
            
            
            Phase2_RedOn();
            
            Phase2_GreenOff();
            Phase2_YellowOff();
            
            // Display time:
            // 1st:
            LcdPrintStringS(0,0,"YELLOW 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0,"RED 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            
            //Switch
            if(timeOfLight <= 0){
                status = PHASE2_GREEN;
                timeOfLight = redTime;
                timeOfLight_2 = green_2_Time;
            }
            break;
        
        case PHASE2_GREEN:
            
            //TODO:
            Phase1_RedOn(); 
            
            Phase1_YellowOff();
            Phase1_GreenOff();
            
            
            Phase2_GreenOn();
            
            Phase2_RedOff();
            Phase2_YellowOff();
            
            // Display time:
            // 1st:
            LcdPrintStringS(0,0,"RED 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0,"GREEN 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            
            //Switch
            if(timeOfLight_2 <= 0){
                status = PHASE2_YELLOW;
                timeOfLight_2 = yellow_2_Time;
            }
            break;
        
        case PHASE2_YELLOW:
            
            //TODO:
            Phase1_RedOn(); 
            
            Phase1_YellowOff();
            Phase1_GreenOff();
            
            
            Phase2_YellowOn();
            
            Phase2_RedOff();
            Phase2_GreenOff();
            
            // Display time:
            // 1st:
            LcdPrintStringS(0,0,"RED 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0," YELLOW 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            
            //Switch
            if(timeOfLight_2 <= 0){
                status = PHASE1_GREEN;
                timeOfLight = green_1_Time;
                timeOfLight_2 = redTime_2;
            }
            break;
        
        default:
            LcdPrintStringS(0,0,"ERROR!");
            LcdPrintNumS(1,0,status);
            break;
            
    }
}