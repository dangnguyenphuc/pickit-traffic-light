#include "global.h"

unsigned int status = INIT_SYSTEM;
unsigned int error = NONE_ERROR;

int timeOfLight = 0;
int timeOfLight_2 = 0;

unsigned char counterAllFSM = 0;

unsigned char timeInManMode = 0;

unsigned int green_1_Time= GREEN_PHASE1_TIME;
unsigned int yellow_1_Time= YELLOW_PHASE1_TIME;
unsigned int redTime = GREEN_PHASE1_TIME + YELLOW_PHASE1_TIME;

unsigned int green_2_Time = GREEN_PHASE2_TIME;
unsigned int yellow_2_Time = YELLOW_PHASE2_TIME;
unsigned int redTime_2 = GREEN_PHASE2_TIME + YELLOW_PHASE2_TIME;

unsigned int temp_green1 = 0;
unsigned int temp_yellow1 = 0;
unsigned int temp_green2 = 0;
unsigned int temp_yellow2 = 0;

unsigned int errorCounter = 0;

char redIsOn = 0;
char yellowIsOn = 0;
char greenIsOn = 0;

char red2IsOn = 0;
char yellow2IsOn = 0;
char green2IsOn = 0;