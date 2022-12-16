#include "main.h"
// Noi khai bao hang so
#define     LED     PORTD
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

//button
unsigned char switchMan();
unsigned char switchTun();
unsigned char backingState();
unsigned char increaseValue();
unsigned char decreaseValue();
unsigned char applyMan();
unsigned char applySetting();
unsigned char slowDownPressed();

// Time counter:
void countTime();

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

// ERROR:
void Error_Handle();

// UART: 
// sending
void UART_sendingStatus();
void UART_sendingTimerLight1(int isError);
void UART_sendingTimerLight2(int isError);

void UART_sendingTimerLight1_MAN();
void UART_sendingTimerLight2_MAN();
void UART_sendingSettngLight1();
void UART_sendingSettngLight2();

void sendLightTimer();
// receiving
void UART_receiving();

//Compare Receive
unsigned char compare(int s1, int s2, int s3, int s4, int s5, int s6);

//Reset system
void ServerPressedReset();

// 1st FSM
void fsm_automatic();
// 2nd FSM
void fsm_manual();
// 3rd FSM
void fsm_tuning();

////////////////////////////////////////////////////////////////////
//Hien thuc cac chuong trinh con, ham, module, function duoi cho nay
////////////////////////////////////////////////////////////////////
void main(void)
{
	int k = 0;
	init_system();
    UART_sendingSettngLight1();
    UART_sendingSettngLight2();
    UART_sendingTimerLight1(0);
    UART_sendingTimerLight2(0);
        lcd_clear();
        LcdClearS();
        delay_ms(1000);
	while (1)
	{
            while (!flag_timer3);
            flag_timer3 = 0;
            //k = k + 1111;
            //UartSendNumToString(k);
            //UartSendString(" ");
            scan_key_matrix_with_uart();
            //BaiTap_UART();
//            GetSensor();
            ServerPressedReset();
            LcdClearS();
            countTime();
            fsm_manual();
            fsm_automatic();
            fsm_tuning();
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

unsigned char compare(int s0, int s1, int s2, int s3, int s4, int s5){
    if (dataReceive[0] == s0 && dataReceive[1] == s1 && dataReceive[2] == s2 && dataReceive[3] == s3 && dataReceive[4] == s4 && dataReceive[5] == s5)
        return 1;
    return 0;
}

void ServerPressedReset(){
	if(flagOfDataReceiveComplete == 1 && compare(82, 83, 58, 48, 48, 48)){ 
		flagOfDataReceiveComplete = 0;
        temp_green1 = GREEN_PHASE1_TIME;
        temp_green2 = GREEN_PHASE2_TIME;
        temp_yellow1 = YELLOW_PHASE1_TIME;
        temp_yellow2 = YELLOW_PHASE2_TIME;
        
        status = INIT_SYSTEM;
		green_1_Time = GREEN_PHASE1_TIME;
		yellow_1_Time = YELLOW_PHASE1_TIME;
		redTime_2 = green_1_Time+yellow_1_Time;
		
		green_2_Time = GREEN_PHASE2_TIME;
		yellow_2_Time = YELLOW_PHASE2_TIME;
		redTime = green_2_Time+yellow_2_Time;
        UART_sendingSettngLight1();
        UART_sendingSettngLight2();
	}
	
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


// ===================================================
// Error
void Error_Handle(){
    switch(error){
        case NONE_ERROR: 
            break;
        case VALUE_OUT_OF_RANGE:
            LcdClearS();
            LcdPrintStringS(0,0,"OUT OF RANGE");
            timeInManMode = TIME_IN_MAN_MODE;
            break;
        case THREE_STEPS_SETTING:
            LcdClearS();
            LcdPrintStringS(0,0,"APPLY IN YELLOW");
            timeInManMode = TIME_IN_MAN_MODE;
            break;
        case CHANGING_RED_1:
            LcdClearS();
            LcdPrintStringS(0,0,"CHANGING RED1");
            timeInManMode = TIME_IN_MAN_MODE;
            break;
        case CHANGING_RED_2:
            LcdClearS();
            LcdPrintStringS(0,0,"CHANGING RED2");
            timeInManMode = TIME_IN_MAN_MODE;
            break;
        case DELAY_TO_SYNC_YELLOW1:
            LcdPrintStringS(0,0,"DELAY YELLOW2   ");
            break;
        case DELAY_TO_SYNC_YELLOW2:
            LcdPrintStringS(1,0,"DELAY YELLOW1   ");
            break;
        case DELAY_TO_SYNC_RED1:
            LcdPrintStringS(0,0,"DELAY RED1      ");
            break;
        case DELAY_TO_SYNC_RED2:
            LcdPrintStringS(1,0,"DELAY RED2      ");
            break;
        default:
            break;
    }
    if(errorCounter == 1){
        error = NONE_ERROR;
        timeInManMode = TIME_IN_MAN_MODE;
    }
}
// ===================================================

// ###################################################

// ===================================================
// led
void Phase1_GreenOn()
{
    OpenOutput(0);
    greenIsOn = 1;
}
void Phase1_GreenOff()
{
    CloseOutput(0);
    greenIsOn = 0;
}

void Phase1_YellowOn()
{
    OpenOutput(4);
    yellowIsOn = 1;
}
void Phase1_YellowOff()
{
    CloseOutput(4);
    yellowIsOn = 0;
}

void Phase1_RedOn()
{
    OpenOutput(6);
    redIsOn = 1;
}
void Phase1_RedOff()
{
    CloseOutput(6);
    redIsOn = 0;
}

void Phase2_GreenOn()
{
    OpenOutput(1);
    green2IsOn = 1;
}
void Phase2_GreenOff()
{
    CloseOutput(1);
    green2IsOn = 0;
}

void Phase2_YellowOn()
{
    OpenOutput(5);
    yellow2IsOn = 1;
}
void Phase2_YellowOff()
{
    CloseOutput(5);
    yellow2IsOn = 0;
}

void Phase2_RedOn()
{
    OpenOutput(7);
    red2IsOn = 1;
}
void Phase2_RedOff()
{
    CloseOutput(7);
    red2IsOn = 0;
}
// ===================================================

// ###################################################

// ===================================================
// buttons
unsigned char switchMan(){
    if(key_code[0] == 1) {
        return 1;
    }
    else{
        return 0;
    }
}

unsigned char switchTun(){
    if(key_code[1] == 1) {
        return 1;
    }
    else{
        return 0;
    }
}

unsigned char backingState(){
    if(key_code[3] == 1) {
        return 1;
    }
    else{
        return 0;
    }
}

unsigned char applyMan(){
    if(key_code[2] == 1) {
        return 1;
    }
    else{
        return 0;
    }
}

unsigned char increaseValue(){
    if(key_code[1] == 1) {
        return 1;
    }
    else{
        return 0;
    }
    
}
unsigned char decreaseValue(){
    if(key_code[1]>=20 && key_code[1] % 10 == 1) {
        return 1;
    }
    else{
        return 0;
    }
}

unsigned char applySetting(){
    if(key_code[2] == 1) {
        return 1;
    }
    else{
        return 0;
    }
}

unsigned char slowDownPressed(){
    if(key_code[3] == 1) {
        return 1;
    }
    else{
        return 0;
    }
}
// ===================================================

// ###################################################

// ===================================================
// UART ==============================================
// sending
void UART_sendingStatus(){
    
    /*
     SENDING : !STATUS:status#
    */
    
    UartSendString("!");
    UartSendString("STATUS:");
    UartSendNumToString(status);
    UartSendString("#");
}

void UART_sendingTimerLight1(int isError){
    
    /*
     SENDING : !LIGHT1:time:red:yellow:green#
    */
    
    UartSendString("!");
    UartSendString("LIGHT1:");
    if(!isError){
       UartSendNumToString(timeOfLight); 
    }else{
        UartSendString("DELAY");
    }
    UartSendString(":");
    UartSendNumToString(redIsOn);
    UartSendString(":");
    UartSendNumToString(yellowIsOn);
    UartSendString(":");
    UartSendNumToString(greenIsOn);
    UartSendString("#");
}

void UART_sendingTimerLight2(int isError){
    
    /*
     SENDING : !LIGHT2:time:red:yellow:green#
    */
    
    UartSendString("!");
    UartSendString("LIGHT2:");
    if(!isError){
       UartSendNumToString(timeOfLight_2); 
    }else{
        UartSendString("DELAY");
    }
    UartSendString(":");
    UartSendNumToString(red2IsOn);
    UartSendString(":");
    UartSendNumToString(yellow2IsOn);
    UartSendString(":");
    UartSendNumToString(green2IsOn);
    UartSendString("#");
}

void UART_sendingTimerLight1_MAN(){
    UartSendString("!");
    UartSendString("LIGHT1:");
    UartSendNumToString(timeInManMode); 
    UartSendString(":");
    UartSendNumToString(redIsOn);
    UartSendString(":");
    UartSendNumToString(yellowIsOn);
    UartSendString(":");
    UartSendNumToString(greenIsOn);
    UartSendString("#");
}

void UART_sendingTimerLight2_MAN(){
    UartSendString("!");
    UartSendString("LIGHT2:");
    UartSendNumToString(timeInManMode); 
    UartSendString(":");
    UartSendNumToString(red2IsOn);
    UartSendString(":");
    UartSendNumToString(yellow2IsOn);
    UartSendString(":");
    UartSendNumToString(green2IsOn);
    UartSendString("#");
}

void UART_sendingSettngLight1(){
        
    /*
     SENDING setting times : !SET1:red:yellow:green#
    */
    
    UartSendString("!");
    UartSendString("SET1:");
    UartSendNumToString(temp_green1 + temp_yellow1);
    UartSendString(":");
    UartSendNumToString(temp_yellow1);
    UartSendString(":");
    UartSendNumToString(temp_green1);
    UartSendString("#");
}

void UART_sendingSettngLight2(){
    
    /*
     SENDING setting times : !SET2:red:yellow:green#
    */
    
    UartSendString("!");
    UartSendString("SET2:");
    UartSendNumToString(temp_green2 + temp_yellow2);
    UartSendString(":");
    UartSendNumToString(temp_yellow2);
    UartSendString(":");
    UartSendNumToString(temp_green2);
    UartSendString("#");
}

void sendLightTimer(){
    if(counterAllFSM == 1){
        if(timeOfLight <= -1){
            UART_sendingTimerLight1(1);
        }
        else{
            UART_sendingTimerLight1(0);
        }
        
        if(timeOfLight_2 <= -1){
            UART_sendingTimerLight2(1);
        }else{
            UART_sendingTimerLight2(0);
        }
    }
}

void sendLightTimer_MAN(){
    if(counterAllFSM == 1){
        UART_sendingTimerLight1_MAN();
        UART_sendingTimerLight2_MAN();
    }
}

// ===================================================
void countTime(){
    errorCounter = (errorCounter+1)%40;
    counterAllFSM = (counterAllFSM+1)%20;
    if(counterAllFSM == 1){
        timeOfLight -= 1;
        timeOfLight_2 -= 1;
        timeInManMode -= 1;
        
        if(timeOfLight < 0){
            timeOfLight = -1;
        }
        
        if(timeOfLight_2 < 0){
            timeOfLight_2 = -1;
        }
        
        if(timeInManMode < 0){
            timeInManMode = -1;
        }
    }
}

void fsm_automatic(){
    switch(status){
        case INIT_SYSTEM:
            
            //TODO:
                // NONE
            
            //Switch
            status = PHASE1_GREEN;
            timeOfLight = green_1_Time;
            timeOfLight_2 = redTime_2;
            UART_sendingStatus();
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
            
            if(timeOfLight_2 == -1){
                error = DELAY_TO_SYNC_RED2;
            }
            
            // UART: 
            sendLightTimer();
            
            // Display times:
            // 1st:
            LcdPrintStringS(0,0,"GREEN 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0,"RED 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            

            
            // Error:
            if(error == NONE_ERROR){
                
                // Display times:
                if(timeOfLight_2 == -1){
                    LcdPrintStringS(1,11,"DELAY");
                }
                
            }else{
                Error_Handle();
            }
            
            //Switch
            // Time out!
            if(timeOfLight <= 0){
                status = PHASE1_YELLOW;
                UART_sendingStatus();
                timeOfLight = yellow_1_Time;
            }
            // Button pressed
            else if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_GREEN1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            else if(switchTun() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
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
            
            if(timeOfLight == -1){
                error = DELAY_TO_SYNC_YELLOW1;
            }
            
            if(timeOfLight_2 == -1){
                error = DELAY_TO_SYNC_RED2;
            }
            
            // UART: 
            sendLightTimer();
            
            // Display time:
            // 1st:
            LcdPrintStringS(0,0,"YELLOW 1:   ");
            LcdPrintNumS(0,13,timeOfLight); 
            // 2nd
            LcdPrintStringS(1,0,"RED 2:   ");  
            LcdPrintNumS(1,13,timeOfLight_2);
          
            // Error:
            if(error == NONE_ERROR){
                
                // Display times:
                if(timeOfLight == -1){
                    LcdPrintStringS(0,11,"DELAY");
                }
                if(timeOfLight_2 == -1){
                    LcdPrintStringS(1,11,"DELAY");
                }
                
            }else{
                Error_Handle();
            }
            
            if(timeOfLight_2 == 0){
                timeOfLight == 0;
            }
            
            if(timeOfLight == 0){
                timeOfLight_2 == 0;
            }
            
            //Switch
            // Time out
            if(timeOfLight <= 0 && timeOfLight_2 <= 0){
                status = PHASE2_GREEN;
                UART_sendingStatus();
                timeOfLight = redTime;
                timeOfLight_2 = green_2_Time;
            }
            
            // Button pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48) )){
                flagOfDataReceiveComplete = 0;
                status = MAN_YELLOW1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
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
            
            if(timeOfLight == -1){
                error = DELAY_TO_SYNC_RED1;
            }
            
            // UART: 
            sendLightTimer();
            
            // Display time:
            // 1st:
            LcdPrintStringS(0,0,"RED 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0,"GREEN 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            
            // Error:
            if(error == NONE_ERROR){
                
                // Display times:
                if(timeOfLight == -1){
                    LcdPrintStringS(0,11,"DELAY");
                }
                
            }else{
                Error_Handle();
            }
            
            //Switch
            // Time out
            if(timeOfLight_2 <= 0){
                status = PHASE2_YELLOW;
                UART_sendingStatus();
                timeOfLight_2 = yellow_2_Time;
            }
            
            // Button pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_GREEN2;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
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
            
            if(timeOfLight == -1){
                error = DELAY_TO_SYNC_RED1;
            }
            
            if(timeOfLight_2 == -1){
                error = DELAY_TO_SYNC_YELLOW2;
            }
            
            // UART: 
            sendLightTimer();
            
            // Display time:
            // 1st:
            LcdPrintStringS(0,0,"RED 1:   ");
            LcdPrintNumS(0,13,timeOfLight);
            // 2nd
            LcdPrintStringS(1,0,"YELLOW 2:   ");
            LcdPrintNumS(1,13,timeOfLight_2);
            
            // Error:
            if(error == NONE_ERROR){
                
                // Display times:
                if(timeOfLight == -1){
                    LcdPrintStringS(0,11,"DELAY");
                }
                if(timeOfLight_2 == -1){
                    LcdPrintStringS(1,11,"DELAY");
                }
                
            }else{
                Error_Handle();
            }
            
            if(timeOfLight_2 == 0){
                timeOfLight == 0;
            }
            
            if(timeOfLight == 0){
                timeOfLight_2 == 0;
            }

            //Switch
            // Time out
            if(timeOfLight_2 <= 0 && timeOfLight <= 0){
                status = PHASE1_GREEN;
                UART_sendingStatus();
                timeOfLight = green_1_Time;
                timeOfLight_2 = redTime_2;
            }
            
            // Button pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48)) ){
                flagOfDataReceiveComplete = 0;
                status = MAN_YELLOW2; 
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48)) ){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
            }
            
           
            break;
            
        case WAIT:
            break;
            
        default:
            break;
            
    }

}

void fsm_manual(){
    switch(status){
        case MAN_GREEN1:
            //TODO:
            Phase1_GreenOn();
            
            Phase1_RedOff();
            Phase1_YellowOff();
            
            
            Phase2_RedOn();
            
            Phase2_GreenOff();
            Phase2_YellowOff();
           
            //UART
            sendLightTimer_MAN();
            
            // Display times:
            // 1st:
            LcdPrintStringS(0,0,"GREEN 1:   ");
            LcdPrintNumS(0,13,timeInManMode);
            // 2nd
            LcdPrintStringS(1,0,"RED 2:   ");
            LcdPrintNumS(1,13,timeInManMode);
            
            //Switch
            // Time out
            if(timeInManMode <= 0){
                status = PHASE1_YELLOW;
                UART_sendingStatus();
                timeOfLight = yellow_1_Time;
                timeOfLight_2 = yellow_1_Time;
            }
            
            // Button Pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_YELLOW1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            if(applyMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = PHASE1_GREEN;
                UART_sendingStatus();
                timeOfLight = green_1_Time;
                timeOfLight_2 = redTime_2;
            }
            
            if(backingState() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_YELLOW2;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
            }
            
            break;
        
        case MAN_YELLOW1:

            //TODO:
            Phase1_YellowOn();
            
            Phase1_GreenOff();
            Phase1_RedOff();
            
            
            Phase2_RedOn();
            
            Phase2_GreenOff();
            Phase2_YellowOff();
            
            //UART
            sendLightTimer_MAN();
            
            // Display times:
            // 1st:
            LcdPrintStringS(0,0,"YELLOW 1:   ");
            LcdPrintNumS(0,13,timeInManMode);
            // 2nd
            LcdPrintStringS(1,0,"RED 2:   ");
            LcdPrintNumS(1,13,timeInManMode);
            
            //Switch
            // Time out
            if(timeInManMode <= 0){
                status = PHASE2_GREEN;
                UART_sendingStatus();
                timeOfLight = redTime;
                timeOfLight_2 = green_2_Time;
            }
            
            // Button Pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_GREEN2;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            if(applyMan()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = PHASE1_YELLOW;
                UART_sendingStatus();
                timeOfLight = yellow_1_Time;
                timeOfLight_2 = yellow_1_Time;
            }
            
            if(backingState()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_GREEN1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
            }
            break;
            
        case MAN_GREEN2:
            
            //TODO:
            Phase2_GreenOn();
            
            Phase2_RedOff();
            Phase2_YellowOff();
            
            
            Phase1_RedOn();
            
            Phase1_GreenOff();
            Phase1_YellowOff();
            
            //UART
            sendLightTimer_MAN();
            
            // Display times:
            // 1st:
            LcdPrintStringS(0,0,"RED 1:   ");
            LcdPrintNumS(0,13,timeInManMode);
            // 2nd
            LcdPrintStringS(1,0,"GREEN 2:   ");
            LcdPrintNumS(1,13,timeInManMode);
            
            //Switch
            // Time out
            if(timeInManMode <= 0){
                status = PHASE2_YELLOW;
                UART_sendingStatus();
                timeOfLight = yellow_2_Time;
                timeOfLight_2 = yellow_2_Time;
            }
            
            // Button Pressed
            if(switchMan()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_YELLOW2;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            if(applyMan()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = PHASE2_GREEN;
                UART_sendingStatus();
                timeOfLight = redTime;
                timeOfLight_2 = green_2_Time;
            }
            
            if(backingState()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_YELLOW1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
            }
            break;
        
        case MAN_YELLOW2:
            
            //TODO:
            Phase2_YellowOn();
            
            Phase2_GreenOff();
            Phase2_RedOff();
            
            
            Phase1_RedOn();
            
            Phase1_GreenOff();
            Phase1_YellowOff();
            
            //UART
            sendLightTimer_MAN();
            
            // Display times:
            // 1st:
            LcdPrintStringS(0,0,"RED 1:   ");
            LcdPrintNumS(0,13,timeInManMode);
            // 2nd
            LcdPrintStringS(1,0,"YELLOW 2:   ");
            LcdPrintNumS(1,13,timeInManMode);
            
            //Switch
            // Time out
            if(timeInManMode == 0){
                status = PHASE1_GREEN;
                UART_sendingStatus();
                timeOfLight = green_1_Time;
                timeOfLight_2 = redTime;
            }
            
            // Button Pressed
            if(switchMan()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 48, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_GREEN1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(switchTun()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = INIT_TUNING;
                UART_sendingStatus();
            }
            
            if(applyMan()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = PHASE2_YELLOW;
                UART_sendingStatus();
                timeOfLight = yellow_2_Time;
                timeOfLight_2 = yellow_2_Time;
            }
            
            if(backingState()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = MAN_GREEN2;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && compare(67, 72, 84, 73, 77, 69)){
                status = INIT_TUNING;
                flagOfDataReceiveComplete = 0;
            }
            break;
            
        default:
            break;
            
    }
}

void fsm_tuning(){
    switch(status){
        
        case INIT_TUNING:
            
            //TODO:
                // NONE
            
            //Switch
            status = TUNING_GREEN1;
            UART_sendingStatus();
            timeInManMode = TIME_IN_MAN_MODE;
            temp_green1 = green_1_Time;
            temp_yellow1 = yellow_1_Time;
            temp_green2 = green_2_Time;
            temp_yellow2 = yellow_2_Time;
            break;
            
        case TUNING_GREEN1:
            
            //TODO:
            // Toggle RED 1 up for 1 sec:
            if(counterAllFSM==1){
                Phase1_RedOn();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOn();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            else{
                Phase1_RedOff();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOff();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            
            sendLightTimer_MAN();
            
            if(increaseValue() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 52, 48, 48)) ){
                flagOfDataReceiveComplete = 0;
                if (temp_green1 < 999){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_green1 += 1;
                    UART_sendingSettngLight1();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(decreaseValue()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 53, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_green1 > 1){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_green1 -= 1;
                    UART_sendingSettngLight1();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(error == NONE_ERROR){
                // Display times:
                LcdPrintStringS(1,0,"GREEN 1:   ");
                LcdPrintNumS(1,13,temp_green1);
            }else{
                Error_Handle();
            }

            //Switch
            // Time out
            if(timeInManMode == 0){
                status = INIT_SYSTEM;
                UART_sendingStatus();
            }
            
            // Button Pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = TUNING_YELLOW1;
                UART_sendingStatus();
                timeInManMode = TIME_IN_MAN_MODE;
            }
            
            if(backingState()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingStatus();
                status = TUNING_YELLOW2;
            }
            
            if(applySetting() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))
                               || (flagOfDataReceiveComplete == 1 && compare(67, 79, 77, 77, 73, 84))){
                flagOfDataReceiveComplete = 0;
                green_1_Time = temp_green1;
                yellow_1_Time = temp_yellow1;
                green_2_Time = temp_green2;
                yellow_2_Time = temp_yellow2;
                redTime_2 = green_1_Time + yellow_1_Time;
                redTime = green_2_Time + yellow_2_Time;
                status = INIT_SYSTEM;
                UART_sendingSettngLight1();
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_green1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingSettngLight1();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_green2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN2;
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_yellow1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW1;
                UART_sendingSettngLight1();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_yellow2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW2;
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            break;
        
        case TUNING_YELLOW1:
            
            //TODO:
            // Toggle RED 1 up for 1 sec:
            if(counterAllFSM==1){
                Phase1_RedOn();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOn();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            else{
                Phase1_RedOff();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOff();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            
            sendLightTimer_MAN();
            
            if(increaseValue() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 52, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_yellow1 < 999){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_yellow1 += 1;
                    UART_sendingSettngLight1();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(decreaseValue() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 53, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_yellow1 > 1){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_yellow1 -= 1;
                    UART_sendingSettngLight1();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
     
            if(error == NONE_ERROR){

                // Display times:
                LcdPrintStringS(1,0,"YELLOW 1:   ");
                LcdPrintNumS(1,13,temp_yellow1);
            }else{
                Error_Handle();
            }

            //Switch
            // Time out
            if(timeInManMode == 0){
                status = INIT_SYSTEM;
                UART_sendingStatus();
            }
            
            // Button Pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = TUNING_GREEN2;
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingStatus();
            }
            
            if(backingState() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                status = TUNING_GREEN1;
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingStatus();
            }
            
            if(applySetting()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))
                             || (flagOfDataReceiveComplete == 1 && compare(67, 79, 77, 77, 73, 84))){
                flagOfDataReceiveComplete = 0;
                green_1_Time = temp_green1;
                yellow_1_Time = temp_yellow1;
                green_2_Time = temp_green2;
                yellow_2_Time = temp_yellow2;
                redTime_2 = green_1_Time + yellow_1_Time;
                redTime = green_2_Time + yellow_2_Time;
                status = INIT_SYSTEM;
                UART_sendingSettngLight1();
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_green1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN1;
                UART_sendingSettngLight1();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_green2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN2;
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_yellow1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingSettngLight1();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_yellow2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW2;
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            break;    
            
        case TUNING_GREEN2:
            
            //TODO:
            // Toggle RED 1 up for 1 sec:
            if(counterAllFSM==1){
                Phase1_RedOn();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOn();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            else{
                Phase1_RedOff();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOff();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            
            sendLightTimer_MAN();
            
            if(increaseValue() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 52, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_green2 < 999){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_green2 += 1;
                    UART_sendingSettngLight2();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(decreaseValue()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 53, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_green2 > 1){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_green2 -= 1;
                    UART_sendingSettngLight2();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(error == NONE_ERROR){
                // Display times:
                LcdPrintStringS(1,0,"GREEN 2:   ");
                LcdPrintNumS(1,13,temp_green2);
            }else{
                Error_Handle();
            }
            
            //Switch
            // Time out
            if(timeInManMode <= 0){
                status = INIT_SYSTEM;
                UART_sendingStatus();
            }
            
            // Button Pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW2;
                UART_sendingStatus();
            }
            if(backingState()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW1;
                UART_sendingStatus();
            }
            
            if(applySetting()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))
                             || (flagOfDataReceiveComplete == 1 && compare(67, 79, 77, 77, 73, 84))){
                flagOfDataReceiveComplete = 0;
                green_1_Time = temp_green1;
                yellow_1_Time = temp_yellow1;
                green_2_Time = temp_green2;
                yellow_2_Time = temp_yellow2;
                redTime_2 = green_1_Time + yellow_1_Time;
                redTime = green_2_Time + yellow_2_Time;
                status = INIT_SYSTEM;
                UART_sendingSettngLight1();
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_green1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN1;
                UART_sendingSettngLight1();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_green2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingSettngLight2();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_yellow1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW1;
                UART_sendingSettngLight1();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_yellow2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW2;
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            break;
        
        case TUNING_YELLOW2:
            
            //TODO:
            // Toggle RED 1 up for 1 sec:
            if(counterAllFSM==1){
                Phase1_RedOn();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOn();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            else{
                Phase1_RedOff();
                Phase1_GreenOff();
                Phase1_YellowOff();
                
                Phase2_RedOff();
                Phase2_GreenOff();
                Phase2_YellowOff();
            }
            
            sendLightTimer_MAN();
            
            if(increaseValue()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 52, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_yellow2 < 999){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_yellow2 += 1;
                    UART_sendingSettngLight2();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(decreaseValue()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 53, 48, 48))){
                flagOfDataReceiveComplete = 0;
                if (temp_yellow2 > 1){
                    timeInManMode = TIME_IN_MAN_MODE;
                    temp_yellow2 -= 1;
                    UART_sendingSettngLight2();
                } else {
                    error = VALUE_OUT_OF_RANGE;
                    errorCounter = 2;
                }
            } 
            
            if(error == NONE_ERROR){
                // Display times:
                LcdPrintStringS(1,0,"YELLOW 2:   ");
                LcdPrintNumS(1,13,temp_yellow2);
            }else{
                Error_Handle();  
            }
            
            //Switch
            // Time out
            if(timeInManMode <= 0){
                status = INIT_SYSTEM;
                UART_sendingStatus();
            }
            
            // Button Pressed
            if(switchMan() || (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 49, 48, 48))){
                flagOfDataReceiveComplete = 0;
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN1;
                UART_sendingStatus();
            }
            
            if(backingState()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 51, 48, 48))){
                flagOfDataReceiveComplete = 0;
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN2;
                UART_sendingStatus();
            }
            
            if(applySetting()|| (flagOfDataReceiveComplete ==1 && compare(66, 84, 58, 50, 48, 48))
                             || (flagOfDataReceiveComplete == 1 && compare(67, 79, 77, 77, 73, 84))){
                flagOfDataReceiveComplete = 0;
                green_1_Time = temp_green1;
                yellow_1_Time = temp_yellow1;
                green_2_Time = temp_green2;
                yellow_2_Time = temp_yellow2;
                redTime_2 = green_1_Time + yellow_1_Time;
                redTime = green_2_Time + yellow_2_Time;
                status = INIT_SYSTEM;
                UART_sendingSettngLight1();
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            //Control by terminal
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_green1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN1;
                UART_sendingSettngLight1();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 71 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_green2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_GREEN2;
                UART_sendingSettngLight2();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 49){
                flagOfDataReceiveComplete = 0;
                temp_yellow1 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                status = TUNING_YELLOW1;
                UART_sendingSettngLight1();
                UART_sendingStatus();
            }
            
            if (flagOfDataReceiveComplete == 1 && dataReceive[0] == 89 && dataReceive[1] == 50){
                flagOfDataReceiveComplete = 0;
                temp_yellow2 = (dataReceive[3] - 48) * 100 + (dataReceive[4] - 48) * 10 + (dataReceive[5] - 48);
                timeInManMode = TIME_IN_MAN_MODE;
                UART_sendingSettngLight2();
            }
            break;
            
        default:
            break;
            
    }
}