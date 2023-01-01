#include "global.h"

unsigned int status = INIT_SYSTEM;
unsigned int error = NONE_ERROR;

int timeOfLight = 0;
int timeOfLight_2 = 0;

unsigned char counterAllFSM = 0;

unsigned char timeInManMode = 0;

unsigned int green_1_Time= GREEN_PHASE1_TIME;
unsigned int yellow_1_Time= YELLOW_PHASE1_TIME;
unsigned int redTime = GREEN_PHASE2_TIME + YELLOW_PHASE2_TIME;

unsigned int green_2_Time = GREEN_PHASE2_TIME;
unsigned int yellow_2_Time = YELLOW_PHASE2_TIME;
unsigned int redTime_2 = GREEN_PHASE1_TIME + YELLOW_PHASE1_TIME;

unsigned int temp_green1 = GREEN_PHASE1_TIME;
unsigned int temp_yellow1 = YELLOW_PHASE1_TIME;
unsigned int temp_green2 = GREEN_PHASE2_TIME;
unsigned int temp_yellow2 = YELLOW_PHASE2_TIME;

unsigned int errorCounter = 0;

char redIsOn = 0;
char yellowIsOn = 0;
char greenIsOn = 0;

char red2IsOn = 0;
char yellow2IsOn = 0;
char green2IsOn = 0;


// timer for stop and wait
int timer_status_ACK = 0;
int timer_setting_ACK = 0;
int timer_light_ACK = 0;


// flags for stop and wait 
char flag_wating_status_ACK = 0;
char flag_waiting_setting_ACK = 0;
char flag_waiting_light_ACK = 0;


// buffers for stop and wait
int buffer_status       = -1;

int buffer_time[2]      = {-1,-1};

int buffer_setting[2][2]   = {{-1,-1},{-1,-1}};

int buffer_led_red[2]   = {-1,-1};

int buffer_led_green[2] = {-1,-1};

int buffer_led_yellow[2]= {-1,-1};

