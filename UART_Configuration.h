/*
 * UART_Configuration.h
 *
 * Created: 9/29/2022 1:01:51 PM
 *  Author: Dell
 */ 


#ifndef UART_CONFIGURATION_H_
#define UART_CONFIGURATION_H_
#include "DIO.h"
#include "Interrupt.h"

// UART Parity Bit 
#define UART_Parity_Mode Parity_Enabled_Even

// UART Data Bits
#define UART_Data_Bits Data_8_Bits

// UART Stop Bits
#define UART_Stop_Bits Stop_1_Bits 

// UART Baud Rate Mode
#define UART_Baud_Rate B_9600
 
//UART Doubled Speed Mode
#define UART_Doubled_Speed Doubled_Speed_Enbaled
 
#endif /* UART_CONFIGURATION_H_ */