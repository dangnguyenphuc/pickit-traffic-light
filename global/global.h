/* 
 * File:   global.h
 * Author: phucd
 *
 * Created on November 28, 2022, 11:04 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#define     INIT_SYSTEM                 255
#define     INIT_TUNING                 511

#define     PHASE1_GREEN                5
#define     PHASE1_YELLOW               6
#define     PHASE2_GREEN                105
#define     PHASE2_YELLOW               106

#define     WAIT                        50

#define     MAN_GREEN1                  15
#define     MAN_YELLOW1                 16
#define     MAN_GREEN2                  115
#define     MAN_YELLOW2                 116

#define     TUNING_RED                  25
#define     TUNING_GREEN                26 
#define     TUNING_YELLOW               27  


#define     GREEN_PHASE1_TIME           10
#define     YELLOW_PHASE1_TIME          5

#define     GREEN_PHASE2_TIME           10
#define     YELLOW_PHASE2_TIME          5

#define     TIME_IN_MAN_MODE            30

// ERROR: ================
#define NONE_ERROR                      0
#define VALUE_OUT_OF_RANGE              1

// =======================


extern unsigned int status;

// ERROR ================
extern unsigned int error;
extern unsigned int errorCounter;
// ======================

extern unsigned int green_1_Time;
extern unsigned int yellow_1_Time;
extern unsigned int redTime;

extern unsigned int green_2_Time;
extern unsigned int yellow_2_Time;
extern unsigned int redTime_2;
// FSM 1 variables
extern unsigned char timeOfLight;
extern unsigned char timeOfLight_2;
extern unsigned char counterAllFSM;

// FSM 2 and FSM 3 variables
extern unsigned char timeInManMode;

#endif	/* GLOBAL_H */

