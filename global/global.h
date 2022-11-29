/* 
 * File:   global.h
 * Author: phucd
 *
 * Created on November 28, 2022, 11:04 PM
 */

#ifndef GLOBAL_H
#define	GLOBAL_H

#define     INIT_SYSTEM                 255

#define     PHASE1_GREEN                5
#define     PHASE1_YELLOW               6
#define     PHASE2_GREEN                105
#define     PHASE2_YELLOW               106

#define     WAIT                        50

#define     MAN_RED                     15
#define     MAN_GREEN                   16
#define     MAN_YELLOW                  17


#define     GREEN_PHASE1_TIME           10
#define     YELLOW_PHASE1_TIME          5

#define     GREEN_PHASE2_TIME           10
#define     YELLOW_PHASE2_TIME          5


extern unsigned int status;


extern unsigned int green_1_Time;
extern unsigned int yellow_1_Time;
extern unsigned int redTime;

extern unsigned int green_2_Time;
extern unsigned int yellow_2_Time;
extern unsigned int redTime_2;

extern unsigned char timeOfLight;
extern unsigned char timeOfLight_2;
extern unsigned char cntOfLight;

#endif	/* GLOBAL_H */

