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

#define     SLOW_DOWN1                  48
#define     SLOW_DOWN2                  49
#define     WAIT                        50

#define     MAN_GREEN1                  15
#define     MAN_YELLOW1                 16
#define     MAN_GREEN2                  115
#define     MAN_YELLOW2                 116

#define     TUNING_RED                  25
#define     TUNING_GREEN                26 
#define     TUNING_YELLOW               27  

#define     TUNING_RED2                  30
#define     TUNING_GREEN2                31 
#define     TUNING_YELLOW2               35 


#define     GREEN_PHASE1_TIME           10
#define     YELLOW_PHASE1_TIME          5

#define     GREEN_PHASE2_TIME           10
#define     YELLOW_PHASE2_TIME          5

#define     TIME_IN_MAN_MODE            30
#define     TIME_FOR_SLOW_DOWN           5
#define     TIME_FOR_PEDESTRIAN         20

// ERROR: ================
#define NONE_ERROR                      0
#define VALUE_OUT_OF_RANGE              1
#define THREE_STEPS_SETTING             2
#define CHANGING_RED_1                  3
#define CHANGING_RED_2                  4
#define DELAY_TO_SYNC_YELLOW1           5
#define DELAY_TO_SYNC_YELLOW2           6
#define DELAY_TO_SYNC_RED1              7
#define DELAY_TO_SYNC_RED2              8
#define DELAY_TO_SYNC_RED1_GREEN        9
#define DELAY_TO_SYNC_RED2_GREEN        10
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
extern int timeOfLight;
extern int timeOfLight_2;
extern unsigned char counterAllFSM;

// FSM 2 and FSM 3 variables
extern unsigned char timeInManMode;

#endif	/* GLOBAL_H */

