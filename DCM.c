/*
 * DCM.c
 *
 * Created: 09/10/2022 5:49:53 PM
 *  Author: sara-
 */ 

#include "DCM.h"
#include "Timer.h"


void DCM_ON(Uint8 motor_name , Uint8 Dir)
{
	if (motor_name==DCM1)
	{
	DIO_SetPin_Dir(DCM_EN_Port,DCM1_EN,PIN_OUT);
	DIO_SetPin_Dir(DCM1_Port,DCM1_PIN1,PIN_OUT);
    DIO_SetPin_Dir(DCM1_Port,DCM1_PIN2,PIN_OUT);
	DIO_SetPin_Value(DCM_EN_Port,DCM1_EN,PIN_ON);
	   if (Dir==counter_clockwise)
	   {
	   DIO_SetPin_Value(DCM1_Port,DCM1_PIN1,PIN_OFF);
	   DIO_SetPin_Value(DCM1_Port,DCM1_PIN2,PIN_ON);
	   }
	   else 
	   {
	   DIO_SetPin_Value(DCM1_Port,DCM1_PIN2,PIN_OFF);
	   DIO_SetPin_Value(DCM1_Port,DCM1_PIN1,PIN_ON);
       }
    }
	   
   else if (motor_name==DCM2)
   {
   DIO_SetPin_Dir(DCM_EN_Port,DCM2_EN,PIN_OUT);
   DIO_SetPin_Dir(DCM1_Port,DCM2_PIN1,PIN_OUT);
   DIO_SetPin_Dir(DCM1_Port,DCM2_PIN2,PIN_OUT);
   DIO_SetPin_Value(DCM_EN_Port,DCM2_EN,PIN_ON);
        if (Dir==counter_clockwise)
		{
        DIO_SetPin_Value(DCM2_Port,DCM2_PIN1,PIN_OFF);
        DIO_SetPin_Value(DCM2_Port,DCM2_PIN2,PIN_ON);
		}
		else 
        
		{
			DIO_SetPin_Value(DCM2_Port,DCM2_PIN2,PIN_OFF);
            DIO_SetPin_Value(DCM2_Port,DCM2_PIN1,PIN_ON);
		}
   }
}
void DCM_Speed( f32 speed_percentage )
{
  #if timer_used_for_PWM==timer2 
	Timer2_Init();
	Timer2_Start();
	PWM2_Gen(speed_percentage); 
	
	#elif timer_used_for_PWM==timer1
	Timer1_Init();
	Timer1_Start();
	PWM1_Gen(speed_percentage);
	
	#elif timer_used_for_PWM==timer0
	Timer0_Init();
	Timer0_Start();
	PWM0_Gen(speed_percentage);
	
	#endif
}
void DCM_OFF(Uint8 motor_name)
{
	if (motor_name==DCM1)
	DIO_SetPin_Value(DCM_EN_Port,DCM1_EN,PIN_OFF);
		 else 
		DIO_SetPin_Value(DCM_EN_Port,DCM2_EN,PIN_OFF);
		
}