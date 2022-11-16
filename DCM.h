/*
 * DCM.h
 *
 * Created: 09/10/2022 5:49:38 PM
 *  Author: sara-
 */ 


#ifndef DCM_H_
#define DCM_H_

#include "DCM_CONFG.h"

#define DCM1    0
#define DCM2    1

#define clockwise           0
#define counter_clockwise   1


#define timer0      0
#define timer1      1
#define timer2      2

void DCM_ON(Uint8 motor_name , Uint8 Dir);
void DCM_Speed( f32 speed_percentage);
void DCM_OFF(Uint8 motor_name);



#endif /* DCM_H_ */