/*
 * UART.c
 *
 * Created: 9/29/2022 1:02:18 PM
 *  Author: Dell
 */ 
#include "UART.h"
#include "servoo.h"
#include "LED.h"
#include "LCD.h"
#include "Buzzer.h"
#include "Relay.h"
#include "Timer.h"
#include <avr/interrupt.h>
#include <util/delay.h>

#define Delay 60

extern Uint16 Temp;

Uint8 UART_GetValue = 0;
Uint8 SYS_Check = 0;
Uint8 A_U_Check = 0;
Uint8 CMD_Check = 0;
Uint8 KP_Counter = 0;
Uint8 PC_Counter = 0;
Uint8 AD_Counter = 4;
Uint8 Dimmer_Array[3] = {0,0,0};
f32 Dimmer_Value = 0; 	 			
Uint8 Temp_Display = 0;
Uint8 First_Check = 0;

extern Uint8 eme_check;

void UART_Init(void)
{
	// Global Interrupt
	SREG |= Global_INT << 7;
	
	// Peripheral Interrupt
	UCSRB = (UART_TX_INT << 6) | (UART_RX_INT << 7) | (UCSRB & 0X3F);
	
	// TX Pin Direction
	DIO_SetPin_Dir(DIO_PORTD, PIN_1, PIN_OUT);
	
	// RX Pin Direction
	DIO_SetPin_Dir(DIO_PORTD, PIN_0, PIN_IN);
	
	// UART Receiver/ Transmitter Enable
	UCSRB = (1 << 3) | (1 << 4) | (UCSRB & 0XE7);
	
	// UART Doubled Speed Mode
	UCSRA = (UART_Doubled_Speed << 1) | (UCSRA & 0XFD);
	
	// Parity Bit Mode
	// Data Bits Mode
	// Stop Bit  Mode
	UCSRC = (1 << 7) | (UART_Data_Bits << 1) | (UART_Stop_Bits << 3) | (UART_Parity_Mode << 4) | (UCSRC & 0X41);
	
	// UART Baud Rate
	UBRRL = (Uint8)UART_Baud_Rate;
	UBRRH = (UART_Baud_Rate >> 8);
}

void UART_TX_Char(Uint8 data)
{
	// Sending Data
	UDR = data;
	
	// Checking the data is sent
	while(!Get_Bit(UCSRA, 5));
}

void UART_TX_Str(Uint8 *str)
{
	for(Uint8 i = 0; str[i] != '\n'; i++)
	{
		UART_TX_Char(str[i]);
	}
}

Uint8 UART_RX_Char(void)
{
	Uint8 Value = 0;
	while(!Get_Bit(UCSRA, 5));
	Value = UDR;
	return Value;
}

ISR(USART_RXC_vect)
{
	Uint8 KP_User[4] = { 0 };
	Uint8 KP_Pass[4] = { 0 };
	Uint8 PC_User[4] = { 0 };
	Uint8 PC_Pass[4] = { 0 };
	Uint8 AD_User[4] = { 0 };
	Uint8 AD_Pass[4] = { 0 };
	
	if(!eme_check)
		Timer2_Start();		
	// Write UART receive handler here
	if(!SYS_Check)
	{
			
		// Commands check
		for(Uint8 i = 0; i < 4; i++)
		{
			while(!Get_Bit(UCSRA,7));
			A_U_Check = UDR;
			
			if(A_U_Check == '1')
			{
				// Admin enter the system
				/*------------->Entering Username<-------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter your username\r\n");
				UART_TX_Str("\r\n");
		
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));       // Polling until the user enter a character
					AD_User[i] = UART_RX_Char();	// Receiving the character in username array
				}
		
				/*------------->Entering Password<-------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter your password\r\n");
				UART_TX_Str("\r\n");
		
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));		// Polling until the user enter a character
					AD_Pass[i] = UART_RX_Char();	// Receiving the character in password array
				}
		
				/*---------------> Checking Username & Password<---------------*/
				for(Uint8 Check_Counter = 0; Check_Counter <= AD_Counter; Check_Counter += 4)
				{
					/*----------------->Username Check<------------------*/
					if(AD_User[0] == EEPROM_ReadByte(5, Check_Counter + 1) && AD_User[1] == EEPROM_ReadByte(5,Check_Counter + 2) && AD_User[2] == EEPROM_ReadByte(5,Check_Counter + 3) && AD_User[3] == EEPROM_ReadByte(5,Check_Counter + 4))
					{
						/*----------------->Password Check<------------------*/
						if(AD_Pass[0] == EEPROM_ReadByte(6,Check_Counter + 1) && AD_Pass[1] == EEPROM_ReadByte(6,Check_Counter + 2) && AD_Pass[2] == EEPROM_ReadByte(6,Check_Counter + 3) && AD_Pass[3] == EEPROM_ReadByte(6,Check_Counter + 4))
						{
							/*----------------->Wright Username & Password<-----------------*/
							UART_TX_Str("\r\n");
							UART_TX_Str("\r\n");
							UART_TX_Str("Admin exist, Welcome!\r\n");
							UART_TX_Str("\r\n");
							Check_Counter = AD_Counter + 1;
							SYS_Check = 1;
							CMD_Check = 1;
							i = 5;
							A_U_Check = 0;
						}
					}
				}
		
				/*----------------->Wrong Username or Password<-----------------*/
				if(!SYS_Check)
				{
					UART_TX_Str("\r\n");
					UART_TX_Str("\r\n");
					UART_TX_Str("Admin does not exist. Please try again\r\n");
					UART_TX_Str("\r\n");
				}
			}
			else if(A_U_Check == '2')
			{
				// PC user enter the system
				/*------------->Entering Username<-------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter your username\r\n");
				UART_TX_Str("\r\n");
			
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));       // Polling until the user enter a character
					PC_User[i] = UART_RX_Char();	// Receiving the character in username array
				}
			
				/*------------->Entering Password<-------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter your password\r\n");
				UART_TX_Str("\r\n");
			
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));		// Polling until the user enter a character
					PC_Pass[i] = UART_RX_Char();	// Receiving the character in password array
				}
			
				/*---------------> Checking Username & Password<---------------*/
				for(Uint8 Check_Counter = 0; Check_Counter <= PC_Counter; Check_Counter += 4)
				{
					/*----------------->Username Check<------------------*/
					if(PC_User[0] == EEPROM_ReadByte(2, Check_Counter + 1) && PC_User[1] == EEPROM_ReadByte(2,Check_Counter + 2) && PC_User[2] == EEPROM_ReadByte(2,Check_Counter + 3) && PC_User[3] == EEPROM_ReadByte(2,Check_Counter + 4))
					{
						/*----------------->Password Check<------------------*/
						if(PC_Pass[0] == EEPROM_ReadByte(3,Check_Counter + 1) && PC_Pass[1] == EEPROM_ReadByte(3,Check_Counter + 2) && PC_Pass[2] == EEPROM_ReadByte(3,Check_Counter + 3) && PC_Pass[3] == EEPROM_ReadByte(3,Check_Counter + 4))
						{
							/*----------------->Wright Username & Password<-----------------*/
							UART_TX_Str("\r\n");
							UART_TX_Str("\r\n");
							UART_TX_Str("User exist, Welcome!\r\n");
							UART_TX_Str("\r\n");
							Check_Counter = PC_Counter + 1;
							SYS_Check = 2;
							CMD_Check = 1;
							i = 5;
							A_U_Check = 0;
						}
					}
				}
			
				/*----------------->Wrong Username or Password<-----------------*/
				if(!SYS_Check)
				{
					UART_TX_Str("\r\n");
					UART_TX_Str("\r\n");
					UART_TX_Str("User does not exist. Please try again\r\n");
					UART_TX_Str("\r\n");
				}	
			}
			
			else
			{
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter command number\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("1.Enter Admin\r\n");
				UART_TX_Str("2.Enter User\r\n");
				UART_TX_Str("\r\n");
				
				i--;
			}
			
			if(i == 2)
			{
				LCD_Clear();
				LCD_Write_String("Firing Alarm!");
				
				while (1)
				{
					Buzzer_ON();
					LED3_ON();
					LED4_ON();
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					_delay_ms(Delay);
					LED3_ON();
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					_delay_ms(Delay);
					
					
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					LED2_ON();
					_delay_ms(Delay);
					
					LED0_OFF();
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_OFF();
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					LED2_ON();
					_delay_ms(Delay);
					
					LED0_OFF();
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					Buzzer_OFF();
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					_delay_ms(Delay);
					
					
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					LED2_ON();
					_delay_ms(Delay);
					
					LED0_OFF();
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					LED2_ON();
					_delay_ms(Delay);
					
					LED0_OFF();
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					LED0_ON();
					_delay_ms(Delay);
					LED0_OFF();
					LED2_ON();
					_delay_ms(Delay);
					LED2_OFF();
					_delay_ms(Delay);
					
					
					
					
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(200);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(500);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(100);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(10);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(200);
					LED0_Toggle();
					LED2_Toggle();
					LED3_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(100);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(Delay);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(1000);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(10);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(10);
					LED2_Toggle();
					LED3_Toggle();
					LED0_Toggle();
					LED1_Toggle();
					LED4_Toggle();
					Buzzer_Toggle();
					_delay_ms(100);
					
					LED0_Toggle();
					Buzzer_ON();
					_delay_ms(250);
					LED0_Toggle();
					Buzzer_Toggle();
					_delay_ms(250);
					LED0_Toggle();
					Buzzer_ON();
					_delay_ms(Delay);
					Buzzer_OFF();
				}
			}
		}
	}
	if(SYS_Check == 1)
	{
		UART_GetValue = UDR;
		
		if(CMD_Check)
		{
			UART_GetValue = 8;
			CMD_Check = 0;
		}
	
		if(UART_GetValue == '1')
		{
			// Open the door
			Servoo_Motor(316);
			if(!eme_check)
			{
				LCD_Clear();
				LCD_Write_String("Door Opened!");
				Timer2_SetDelay(2000);
			}
		}
		
		else if(UART_GetValue == '2')
		{
			// Close the door
			Servoo_Motor(97);
			if(!eme_check)
			{
				LCD_Clear();
				LCD_Write_String("Door Closed!");
				Timer2_SetDelay(2000);
			}
		}
		
		else if(UART_GetValue == '3')
		{
			// Open lamps
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Which lamp do you want to be opened(From 1 to 5)\r\n");
			UART_TX_Str("6.All Lamps\r\n");
			UART_TX_Str("\r\n");
			
			while(!Get_Bit(UCSRA,7));			// Polling until the user enter a character
			UART_GetValue = UART_RX_Char();		// Receiving the command
			
			if(UART_GetValue == '1')
			{	
				LED0_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(1) Opened");
				}
			}
			else if(UART_GetValue == '2')
			{
				LED1_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(2) Opened");
				}
			}
			else if(UART_GetValue == '3')		
			{
				LED2_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(3) Opened");
				}
			}
			else if(UART_GetValue == '4')
			{
				LED3_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(4) Opened");
				}
			}
			else if(UART_GetValue == '5')
			{
				LED4_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(5) Opened");
				}
			}
			else if(UART_GetValue == '6')
			{	
				LED0_ON();
				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("All Lamps Are");
					LCD_Goto(1,0);
					LCD_Write_String("Opened");
				}
			}
			Timer2_SetDelay(2000);
		}
		
		else if(UART_GetValue == '4')
		{
			// Close lamps
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Which lamp do you want to be Closed(From 1 to 5)\r\n");
			UART_TX_Str("6.All Lamps\r\n");
			UART_TX_Str("\r\n");
			
			while(!Get_Bit(UCSRA,7));			// Polling until the user enter a character
			UART_GetValue = UART_RX_Char();	// Receiving the command
			
			if(UART_GetValue == '1')
			{
				LED0_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(1) Closed");	
				}
			}
			else if(UART_GetValue == '2')
			{
				LED1_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(2) Closed");
				}
			}
			else if(UART_GetValue == '3')
			{
				LED2_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(3) Closed");
				}
			}
			else if(UART_GetValue == '4')
			{
				LED3_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(4) Closed");
				}
			}
			else if(UART_GetValue == '5')
			{
				LED4_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(5) Closed");
				}
			}
			else if(UART_GetValue == '6')
			{
				LED0_OFF();
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LCD_Clear();
				if(!eme_check)
				{
					LCD_Write_String("All Lamps Are");
					LCD_Goto(1,0);
					LCD_Write_String("Closed");
				}
			}
			Timer2_SetDelay(2000);
		}
		
		else if(UART_GetValue == '5')
		{
			// Dimmer Circuit
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Please enter the percentage of the lamp\r\n");
			UART_TX_Str("\r\n");
			
			Dimmer_Value = 0;
			
			for(Uint8 i = 0; i < 3; i++)
			{
				while(!Get_Bit(UCSRA,7));
				Dimmer_Array[i] = UART_RX_Char();
				
				if(Dimmer_Array[i] >= 48 && Dimmer_Array[i] <= 57)
				{
					Dimmer_Value = Dimmer_Value*10 + (Dimmer_Array[i]-48);
				}
				
				else
				{
					i = 5;
				}
				
			}
			
			if(!eme_check)
			{
				LCD_Clear();
				LCD_Write_String("Lamp ON!");
				LCD_Goto(1,0);
				LCD_Write_String("Percentage:");
				LCD_Write_Integer(Dimmer_Value);
				LCD_Write_Char('%');
				Timer2_SetDelay(2000);
			}
			
			if(Dimmer_Value >= 58 && Dimmer_Value <= 60)
			{
				Dimmer_Value = 55;
			}
			else if(Dimmer_Value > 60 && Dimmer_Value <= 62)
			{
				Dimmer_Value = 65;
			}
			
			PWM0_Gen(Dimmer_Value);
		}
		
		else if(UART_GetValue == '6')
		{
			// Display temperature
			Uint8 Temp_array[2] = { 0 };			
			Uint8 Temp_Divide = Temp;
			
			for(Uint8 i = 0; i < 2; i++)
			{
				Temp_array[i] = Temp_Divide % 10;
				Temp_Divide /= 10;
			}
			
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Temperature:\r\n");
			UART_TX_Char(Temp_array[1]+48);
			UART_TX_Char(Temp_array[0]+48);
			UART_TX_Str("\r\n");
			
			if(!eme_check)
			{
				LCD_Clear();
				LCD_Write_String("Temperature:");
				LCD_Write_Integer(Temp);
				Timer2_SetDelay(2000);
			}
		}
		
		else if(UART_GetValue == '7')
		{
			// Add new user
			
			/*-------------->Determine The Type of User<---------------*/
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("1.Keypad user\r\n");
			UART_TX_Str("2.PC user\r\n");
			UART_TX_Str("3.AD user\r\n");
			
			while(!Get_Bit(UCSRA,7));			// Polling until the user enter a character
			UART_GetValue = UART_RX_Char();	// Receiving the command
			
			if(UART_GetValue == '1')
			{
				/*--------------->Adding Keypad User<-------------------*/
				Uint8 j = 0;
				
				/*----------------->Adding New Username<-----------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter 4 numbers as username\r\n");
				for(Uint8 i = 0; i < 4; i++)
				{
					j = i + 1;
					while(!Get_Bit(UCSRA,7));							// Polling until the user enter a character
					KP_User[i] = UART_RX_Char();						// Receiving the character in username array
					EEPROM_WriteByte(0,j += KP_Counter,KP_User[i]);		// Adding the username in EEPROM
				}
				
				j = 0;
				
				/*----------------->Adding New Password<-----------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter 4 numbers as password\r\n");
				for(Uint8 i = 0; i < 4; i++)
				{
					j = i + 1;
					while(!Get_Bit(UCSRA,7));							// Polling until the user enter a character
					KP_Pass[i] = UART_RX_Char();						// Receiving the character in password array
					EEPROM_WriteByte(1,j += KP_Counter,KP_Pass[i]);		// Adding the password in EEPROM
				}
				
				KP_Counter += 4;					// Counter to determine the number of keypad users
				EEPROM_WriteByte(4,0,KP_Counter);	// Saving the counter in EEPROM
			}
			
			else if(UART_GetValue == '2')
			{
				/*--------------->Adding PC User<-------------------*/
				Uint8 j = 0;
				
				/*----------------->Adding New Username<-----------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter 4 numbers as username\r\n");
				for(Uint8 i = 0; i < 4; i++)
				{
					j = i + 1;
					while(!Get_Bit(UCSRA,7));							// Polling until the user enter a character
					PC_User[i] = UART_RX_Char();						// Receiving the character in username array
					EEPROM_WriteByte(2,j += PC_Counter,PC_User[i]);		// Adding the username in EEPROM
				}
				
				j = 0;
				
				/*----------------->Adding New Password<-----------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter 4 numbers as password\r\n");
				for(Uint8 i = 0; i < 4; i++)
				{
					j = i + 1;
					while(!Get_Bit(UCSRA,7));							// Polling until the user enter a character
					PC_Pass[i] = UART_RX_Char();						// Receiving the character in password array
					EEPROM_WriteByte(3,j += PC_Counter,PC_Pass[i]);		// Adding the password in EEPROM
				}
				
				PC_Counter += 4;					// Counter to determine the number of PC users
				EEPROM_WriteByte(4,1,PC_Counter);	// Saving the counter in EEPROM
			}
			
			else if(UART_GetValue == '3')
			{
				/*--------------->Adding Admin User<-------------------*/
				Uint8 j = 0;
				
				/*----------------->Adding New Username<-----------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter 4 numbers as username\r\n");
				for(Uint8 i = 0; i < 4; i++)
				{
					j = i + 1;
					while(!Get_Bit(UCSRA,7));							// Polling until the user enter a character
					AD_User[i] = UART_RX_Char();						// Receiving the character in username array
					EEPROM_WriteByte(5,j += AD_Counter,AD_User[i]);		// Adding the username in EEPROM
				}
				
				j = 0;
				
				/*----------------->Adding New Password<-----------------*/
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter 4 numbers as password\r\n");
				for(Uint8 i = 0; i < 4; i++)
				{
					j = i + 1;
					while(!Get_Bit(UCSRA,7));							// Polling until the user enter a character
					AD_Pass[i] = UART_RX_Char();						// Receiving the character in password array
					EEPROM_WriteByte(6,j += AD_Counter,AD_Pass[i]);		// Adding the password in EEPROM
				}
				
				AD_Counter += 4;					// Counter to determine the number of PC users
				EEPROM_WriteByte(4,2,AD_Counter);	// Saving the counter in EEPROM
			}
			
			/*------------>Added Successfully<-------------------*/
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("User added successfully\r\n");
		}

		else if(UART_GetValue == '8')
		{
			// Delete an exist user
			Uint8 j = 0;
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Do you want to delete keypad user , PC user or Admin user\r\n");
			UART_TX_Str("1.Keypad User\r\n");
			UART_TX_Str("2.PC User\r\n");
			UART_TX_Str("3.AD User\r\n");
			
			while(!Get_Bit(UCSRA,7));
			UART_GetValue = UART_RX_Char();
			
			if(UART_GetValue == '1')
			{
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter the KP username\r\n");
				
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));
					KP_User[i] = UART_RX_Char();
				}
				
				for(Uint8 Check_Counter = 0; Check_Counter <= KP_Counter; Check_Counter += 4)
				{
					if(KP_User[0] == EEPROM_ReadByte(0, Check_Counter + 1) && KP_User[1] == EEPROM_ReadByte(0,Check_Counter + 2) && KP_User[2] == EEPROM_ReadByte(0,Check_Counter + 3) && KP_User[3] == EEPROM_ReadByte(0,Check_Counter + 4))
					{
						EEPROM_WriteByte(0,Check_Counter + 1, 255);
						EEPROM_WriteByte(0,Check_Counter + 2, 255);
						EEPROM_WriteByte(0,Check_Counter + 3, 255);
						EEPROM_WriteByte(0,Check_Counter + 4, 255);
						EEPROM_WriteByte(1,Check_Counter + 1, 255);
						EEPROM_WriteByte(1,Check_Counter + 2, 255);
						EEPROM_WriteByte(1,Check_Counter + 3, 255);
						EEPROM_WriteByte(1,Check_Counter + 4, 255);
						
						for(Uint8 Check_Counter_2 = Check_Counter + 1; Check_Counter_2 <= KP_Counter; Check_Counter_2++)
						{
							EEPROM_WriteByte(0, Check_Counter_2, EEPROM_ReadByte(0, Check_Counter_2 + 4));
							EEPROM_WriteByte(1, Check_Counter_2, EEPROM_ReadByte(1, Check_Counter_2 + 4));
						}
						
						KP_Counter -= 4;
						EEPROM_WriteByte(4,0,KP_Counter);
						Check_Counter = KP_Counter + 1;
						UART_TX_Str("\r\n");
						UART_TX_Str("\r\n");
						UART_TX_Str("User deleted successfully\r\n");
						j = 1;
					}
				}
				if(!j)
				{
					UART_TX_Str("\r\n");
					UART_TX_Str("\r\n");
					UART_TX_Str("User is not exist\r\n");
				}
			}
			
			else if(UART_GetValue == '2')
			{
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter the PC username\r\n");
				
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));
					PC_User[i] = UART_RX_Char();
				}
				
				for(Uint8 Check_Counter = 0; Check_Counter <= PC_Counter; Check_Counter += 4)
				{
					if(PC_User[0] == EEPROM_ReadByte(2, Check_Counter + 1) && PC_User[1] == EEPROM_ReadByte(2,Check_Counter + 2) && PC_User[2] == EEPROM_ReadByte(2,Check_Counter + 3) && PC_User[3] == EEPROM_ReadByte(2,Check_Counter + 4))
					{
						EEPROM_WriteByte(2,Check_Counter + 1, 255);
						EEPROM_WriteByte(2,Check_Counter + 2, 255);
						EEPROM_WriteByte(2,Check_Counter + 3, 255);
						EEPROM_WriteByte(2,Check_Counter + 4, 255);
						EEPROM_WriteByte(3,Check_Counter + 1, 255);
						EEPROM_WriteByte(3,Check_Counter + 2, 255);
						EEPROM_WriteByte(3,Check_Counter + 3, 255);
						EEPROM_WriteByte(3,Check_Counter + 4, 255);
						
						for(Uint8 Check_Counter_2 = Check_Counter + 1; Check_Counter_2 <= PC_Counter; Check_Counter_2++)
						{
							EEPROM_WriteByte(2, Check_Counter_2, EEPROM_ReadByte(2, Check_Counter_2 + 4));
							EEPROM_WriteByte(3, Check_Counter_2, EEPROM_ReadByte(3, Check_Counter_2 + 4));
						}
						
						PC_Counter -= 4;
						EEPROM_WriteByte(4,1,PC_Counter);
						Check_Counter = PC_Counter + 1;
						UART_TX_Str("\r\n");
						UART_TX_Str("\r\n");
						UART_TX_Str("User deleted successfully\r\n");
						j = 1;
					}
				}
				if(!j)
				{
					UART_TX_Str("\r\n");
					UART_TX_Str("\r\n");
					UART_TX_Str("User is not exist\r\n");
				}
			}
		
			else if(UART_GetValue = '3')
			{
				UART_TX_Str("\r\n");
				UART_TX_Str("\r\n");
				UART_TX_Str("Please enter the AD username\r\n");
				
				for(Uint8 i = 0; i < 4; i++)
				{
					while(!Get_Bit(UCSRA,7));
					AD_User[i] = UART_RX_Char();
				}
				
				for(Uint8 Check_Counter = 0; Check_Counter <= AD_Counter; Check_Counter += 4)
				{
					if(AD_User[0] == EEPROM_ReadByte(5, Check_Counter + 1) && AD_User[1] == EEPROM_ReadByte(5,Check_Counter + 2) && AD_User[2] == EEPROM_ReadByte(5,Check_Counter + 3) && AD_User[3] == EEPROM_ReadByte(5,Check_Counter + 4))
					{
						EEPROM_WriteByte(5,Check_Counter + 1, 255);
						EEPROM_WriteByte(5,Check_Counter + 2, 255);
						EEPROM_WriteByte(5,Check_Counter + 3, 255);
						EEPROM_WriteByte(5,Check_Counter + 4, 255);
						EEPROM_WriteByte(6,Check_Counter + 1, 255);
						EEPROM_WriteByte(6,Check_Counter + 2, 255);
						EEPROM_WriteByte(6,Check_Counter + 3, 255);
						EEPROM_WriteByte(6,Check_Counter + 4, 255);
						
						for(Uint8 Check_Counter_2 = Check_Counter + 1; Check_Counter_2 <= AD_Counter; Check_Counter_2++)
						{
							EEPROM_WriteByte(5, Check_Counter_2, EEPROM_ReadByte(5, Check_Counter_2 + 4));
							EEPROM_WriteByte(6, Check_Counter_2, EEPROM_ReadByte(6, Check_Counter_2 + 4));
						}
						
						AD_Counter -= 4;
						if(Check_Counter == 0)
						{
							AD_Counter += 4;
						}
						EEPROM_WriteByte(4,2,AD_Counter);
						Check_Counter = AD_Counter + 1;
						UART_TX_Str("\r\n");
						UART_TX_Str("\r\n");
						UART_TX_Str("User deleted successfully\r\n");
						j = 1;
					}
				}
				if(!j)
				{
					UART_TX_Str("\r\n");
					UART_TX_Str("\r\n");
					UART_TX_Str("User is not exist\r\n");
				}
			}
		}
		
		else if(UART_GetValue == 8)
		{
			
		}
		
		else
		{
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Please enter command number\r\n");
		}
		
		UART_TX_Str("\r\n");
		UART_TX_Str("Waiting for your command\r\n");
		UART_TX_Str("\r\n");
		UART_TX_Str("1.Open the door\r\n");
		UART_TX_Str("2.Close the door\r\n");
		UART_TX_Str("3.Open lamps\r\n");
		UART_TX_Str("4.Close lamps\r\n");
		UART_TX_Str("5.Variant lamp\r\n");
		UART_TX_Str("6.Display the temperature\r\n");
		UART_TX_Str("7.Add new user\r\n");
		UART_TX_Str("8.Delete exist user\r\n");
		UART_TX_Str("Reply with the number of the command\r\n");
		UART_TX_Str("\r\n");
	}
	else if(SYS_Check == 2)
	{
		UART_GetValue = UDR;
		
		if(CMD_Check)
		{
			UART_GetValue = 8;
			CMD_Check = 0;
		}
		
		if(UART_GetValue == '1')
		{
			// Open lamps
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Which lamp do you want to be opened(From 1 to 5)\r\n");
			UART_TX_Str("6.All Lamps\r\n");
			UART_TX_Str("\r\n");
			
			while(!Get_Bit(UCSRA,7));			// Polling until the user enter a character
			UART_GetValue = UART_RX_Char();	// Receiving the command
			
			if(UART_GetValue == '1')
			{
				LED0_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(1) Opened");
				}
			}
			else if(UART_GetValue == '2')
			{
				LED1_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(2) Opened");
				}
			}
			else if(UART_GetValue == '3')
			{
				LED2_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(3) Opened");
				}
			}
			else if(UART_GetValue == '4')
			{
				LED3_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(4) Opened");
				}
			}
			else if(UART_GetValue == '5')
			{
				LED4_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(5) Opened");
				}
			}
			else if(UART_GetValue == '6')
			{
				LED0_ON();
				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("All Lamps Are");
					LCD_Goto(1,0);
					LCD_Write_String("Opened");
				}
			}
			Timer2_SetDelay(2000);
		}
		
		else if(UART_GetValue == '2')
		{
			// Close lamps
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Which lamp do you want to be Closed(From 1 to 5)\r\n");
			UART_TX_Str("6.All Lamps\r\n");
			UART_TX_Str("\r\n");
			
			while(!Get_Bit(UCSRA,7));			// Polling until the user enter a character
			UART_GetValue = UART_RX_Char();	// Receiving the command
			
			if(UART_GetValue == '1')
			{
				LED0_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(1) Closed");
				}
			}
			else if(UART_GetValue == '2')
			{
				LED1_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(2) Closed");
				}
			}
			else if(UART_GetValue == '3')
			{
				LED2_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(3) Closed");
				}
			}
			else if(UART_GetValue == '4')
			{
				LED3_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(4) Closed");
				}
			}
			else if(UART_GetValue == '5')
			{
				LED4_OFF();
				if(!eme_check)
				{
					LCD_Clear();
					LCD_Write_String("Lamp(5) Closed");
				}
			}
			else if(UART_GetValue == '6')
			{
				LED0_OFF();
				LED1_OFF();
				LED2_OFF();
				LED3_OFF();
				LED4_OFF();
				LCD_Clear();
				if(!eme_check)
				{
					LCD_Write_String("All Lamps Are");
					LCD_Goto(1,0);
					LCD_Write_String("Closed");
				}
			}
			Timer2_SetDelay(2000);
		}
		
		else if(UART_GetValue == '3')
		{
			// Dimmer Circuit
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Please enter the percentage of the lamp\r\n");
			UART_TX_Str("\r\n");
			
			Dimmer_Value = 0;
			
			for(Uint8 i = 0; i < 3; i++)
			{
				while(!Get_Bit(UCSRA,7));
				Dimmer_Array[i] = UART_RX_Char();
				
				if(Dimmer_Array[i] >= 48 && Dimmer_Array[i] <= 57)
				{
					Dimmer_Value = Dimmer_Value*10 + (Dimmer_Array[i]-48);
				}
				
				else
				{
					i = 5;
				}
				
			}
			
			if(!eme_check)
			{
				LCD_Clear();
				LCD_Write_String("Lamp ON!");
				LCD_Goto(1,0);
				LCD_Write_String("Percentage:");
				LCD_Write_Integer(Dimmer_Value);
				LCD_Write_Char('%');
				Timer2_SetDelay(2000);
			}
			
			if(Dimmer_Value >= 58 && Dimmer_Value <= 60)
			{
				Dimmer_Value = 55;
			}
			else if(Dimmer_Value > 60 && Dimmer_Value <= 62)
			{
				Dimmer_Value = 65;
			}
			
			PWM0_Gen(Dimmer_Value);
		}
		
		else if(UART_GetValue == '4')
		{
			// Display temperature
			Uint8 Temp_array[2] = { 0 };
			Uint8 Temp_Divide = Temp;
			
			for(Uint8 i = 0; i < 2; i++)
			{
				Temp_array[i] = Temp_Divide % 10;
				Temp_Divide /= 10;
			}
			
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Temperature:\r\n");
			UART_TX_Char(Temp_array[1]+48);
			UART_TX_Char(Temp_array[0]+48);
			UART_TX_Str("\r\n");
			
			
			if(!eme_check)
			{
				LCD_Clear();
				LCD_Write_String("Temperature:");
				LCD_Write_Integer(Temp);
				Timer2_SetDelay(2000);
			}
		}
		
		else if(UART_GetValue == 8)
		{
			
		}
		
		else
		{
			UART_TX_Str("\r\n");
			UART_TX_Str("\r\n");
			UART_TX_Str("Please enter command number\r\n");
		}
		
		UART_TX_Str("\r\n");
		UART_TX_Str("\r\n");
		UART_TX_Str("Waiting for your command\r\n");
		UART_TX_Str("\r\n");
		UART_TX_Str("1.Open lamps\r\n");
		UART_TX_Str("2.Close lamps\r\n");
		UART_TX_Str("3.Variant lamp\r\n");
		UART_TX_Str("4.Display the temperature\r\n");
		UART_TX_Str("Reply with the number of the command\r\n");
		UART_TX_Str("\r\n");
	}	
}

ISR(USART_TXC_vect)
{
	// Write UART transmit handler here
	
}

