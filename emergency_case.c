/*
 * CFile1.c
 *
 * Created: 22/10/2022 3:16:23 PM
 *  Author: sara-
 */ 



#include "emergency_case.h"


extern Uint8 button;
extern Uint16 Temp;
extern f32 Dimmer_Value;
extern Uint8 Dimmer_Array[3];
extern Uint8 eme_check;

void emergency_case(void)
{
	LCD_Clear();
	LCD_Write_String("1.open lamps");
	LCD_Goto(1,0);
	LCD_Write_String("2.more");
	button=NULL_KEYPAD;
	
	while(!(button == '1' || button == '2' ) && eme_check)
	button=Keypad_GetValue();
	
	
	if(button=='1')
	{
		LCD_Clear();
		LCD_Write_String("1.lamp1  2.lamp2");
		LCD_Goto(1,0);
		LCD_Write_String("3.lamp3  4.more");
		
		button=NULL_KEYPAD;
		while(!(button == '1' || button == '2' || button == '3' || button == '4') && eme_check)
		button=Keypad_GetValue();
		
		if(button == '1')
		{
			LED0_ON();
			
			
		}
		else if(button == '2')
		{
			LED1_ON();
			
			
		}
		else if(button == '3')
		{
			LED2_ON();
			
		}
		else if(button == '4')
		{
			LCD_Clear();
			LCD_Write_String("1.lamp4  2.lamp5");
			LCD_Goto(1,0);
			LCD_Write_String("3.all lamps");
			
			button=NULL_KEYPAD;
			while(!(button == '1' || button == '2' || button == '3' ) && eme_check)
			button=Keypad_GetValue();
			if (button == '1')
			{
				LED3_ON();
				
			}
			else if(button == '2')
			{
				LED4_ON();
				
			}
			else if(button == '3')
			{
				LED0_ON();
				LED1_ON();
				LED2_ON();
				LED3_ON();
				LED4_ON();
				
			}
			
		}

	}
	
	else if(button=='2')

	{
		LCD_Clear();
		LCD_Write_String("3.close lamps");
		LCD_Goto(1,0);
		LCD_Write_String("4.more");
		button=NULL_KEYPAD;
		while(!(button == '3' || button == '4' ) && eme_check)
		button=Keypad_GetValue();
		
		if(button == '3')
		{
			LCD_Clear();
			LCD_Write_String("1.lamp1  2.lamp2");
			LCD_Goto(1,0);
			LCD_Write_String("3.lamp3  4.more");
			
			button=NULL_KEYPAD;
			while(!(button == '1' || button == '2' || button == '3' || button == '4' ) && eme_check)
			button=Keypad_GetValue();
			
			if(button == '1')
			{
				LED0_OFF();
				
			}
			else if(button == '2')
			{
				LED1_OFF();
				
			}
			else if(button == '3')
			{
				LED2_OFF();
				
			}
			
			
			else if(button== '4')
			{
				LCD_Clear();
				LCD_Write_String("1.lamp4  2.lamp5");
				LCD_Goto(1,0);
				LCD_Write_String("3.all lamps");
				
				button=NULL_KEYPAD;
				while(!(button == '1' || button == '2' || button == '3' ) && eme_check)
				button=Keypad_GetValue();
				
				if(button=='1')
				{
					LED3_OFF();
					
				}
				
				else if(button == '2')
				{
					LED4_OFF();
					
				}
				else if(button == '3')
				{
					LED0_OFF();
					LED1_OFF();
					LED2_OFF();
					LED3_OFF();
					LED4_OFF();
					
				}
			}
			
		}
		
		
		
		else if(button=='4')
		{
			LCD_Clear();
			LCD_Write_String("5.variant lamp");
			LCD_Goto(1,0);
			LCD_Write_String("6.more");
			button=NULL_KEYPAD;
			while(!(button == '5' || button == '6' ) && eme_check)
			button=Keypad_GetValue();
			
			if(button =='5')
			{
				LCD_Clear();
				LCD_Write_String("Please enter the");
				LCD_Goto(1,0);
				LCD_Write_String("% of the lamp");
				button=NULL_KEYPAD;
				Dimmer_Value = 0;
				
				for(Uint8 i = 0; i < 3; i++)
				{
					button=NULL_KEYPAD;
					
					while(!(button >= '0' && button <= '9' || button == '*' || button == '#') && eme_check)
					{
						button=Keypad_GetValue();
					}
					
					Dimmer_Array[i] = button;
					
					if(Dimmer_Array[i] >= 48 && Dimmer_Array[i] <= 57)
					{
						Dimmer_Value = Dimmer_Value*10 + (Dimmer_Array[i]-48);
					}
					else
					{
						i = 5;
					}
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
			
			else if(button=='6')
			{
				LCD_Clear();
				LCD_Write_String("7.Display Temp");
				LCD_Goto(1,0);
				LCD_Write_String("8.Exit system");
				
				button = NULL_KEYPAD;
				while(!(button == '7' || button == '8') && eme_check)
					button = Keypad_GetValue();
					
				if(button=='7')
				{
					// Display temperature
					LCD_Clear();
					LCD_Write_String("Temperature:");
					LCD_Write_Integer(Temp);
					_delay_ms(1000);
				}
				
				else if(button == '8')
				{
					eme_check = 0;
					LCD_Clear();
				}	
			}
		}
	}
}
