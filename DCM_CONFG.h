/*
 * DCM_CONFG.h
 *
 * Created: 09/10/2022 5:51:01 PM
 *  Author: sara-
 */ 


#ifndef DCM_CONFG_H_
#define DCM_CONFG_H_

#include "DIO.h"

#define DCM1_Port    DIO_PORTD
#define DCM2_Port    DIO_PORTD
#define DCM_EN_Port  DIO_PORTD

#define DCM1_EN      PIN_3
#define DCM2_EN      PIN_1

#define DCM1_PIN1    PIN_4
#define DCM1_PIN2    PIN_6

#define DCM2_PIN1    PIN_2
#define DCM2_PIN2    PIN_3

#define timer_used_for_PWM    timer2

#endif /* DCM_CONFG_H_ */