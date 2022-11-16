/*
 * Button.c
 *
 * Created: 9/8/2022 3:28:48 PM
 *  Author: Dell
 */ 
#include "Button.h"
#include <util/delay.h>

void Button0_Init(void)
{
	DIO_SetPin_Dir(Button0_Port, Button0_Pin, PIN_IN);
	#if Button0_INIT_STATE == PIN_INTERNAL_PULLUP
	DIO_SetPin_Pullup(Button0_Port, Button0_Pin);
	#elif Button0_INIT_STATE == PIN_INTERNAL_PULLDOWN
	//Do Nothing
	#elif Button0_INIT_STATE == PIN_FLOATED
	//Do Nothing
	#endif
}
Uint8 Button0_GetValue(void)
{
	Uint8 Button0_Value = 0;
	Button0_Value = DIO_GetPin_Value(Button0_Port,Button0_Pin);
	while(DIO_GetPin_Value(Button0_Port,Button0_Pin))
	_delay_ms(20);
	return Button0_Value;
}

void Button1_Init(void)
{
	DIO_SetPin_Dir(Button1_Port, Button1_Pin, PIN_IN);
	#if Button1_INIT_STATE == PIN_INTERNAL_PULLUP
	DIO_SetPin_Pullup(Button1_Port, Button1_Pin);
	#elif Button1_INIT_STATE == PIN_INTERNAL_PULLDOWN
	//Do Nothing
	#elif Button1_INIT_STATE == PIN_FLOATED
	//Do Nothing
	#endif
}
Uint8 Button1_GetValue(void)
{
	Uint8 Button1_Value = 0;
	Button1_Value = DIO_GetPin_Value(Button1_Port,Button1_Pin);
	while(DIO_GetPin_Value(Button1_Port,Button1_Pin))
	_delay_ms(20);
	return Button1_Value;
}

void Button2_Init(void)
{
	DIO_SetPin_Dir(Button2_Port, Button2_Pin, PIN_IN);
	#if Button2_INIT_STATE == PIN_INTERNAL_PULLUP
	DIO_SetPin_Pullup(Button2_Port, Button2_Pin);
	#elif Button2_INIT_STATE == PIN_INTERNAL_PULLDOWN
	//Do Nothing
	#elif Button2_INIT_STATE == PIN_FLOATED
	//Do Nothing
	#endif
}
Uint8 Button2_GetValue(void)
{
	Uint8 Button2_Value = 0;
	Button2_Value = DIO_GetPin_Value(Button2_Port,Button2_Pin);
	while(DIO_GetPin_Value(Button2_Port,Button2_Pin))
	_delay_ms(20);
	return Button2_Value;
}
