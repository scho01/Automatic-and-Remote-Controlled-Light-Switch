#include "stm32l476xx.h"
#include "SysClock.h"
#include "RTC.h"
#include "PWM.h"
#include "LCD.h"
#include "I2C.h"
#include "Light.h"
#include "IR.h"
#include "UART.h"
#include "stdlib.h"

int main() {
	System_Clock_Init();
	LCD_Initialization();
	I2C_GPIO_Init();
	I2C_Initialization();
	Light_Init();
	RTC_Init();
	PWM_Init();
	IR_Init();
	USART_Init();
	uint8_t hour, minute;
	bool door, prevdoor, doorstate;
	char strTime[12] = {0};
	char strDate[12] = {0};
	state = 0;
	while(1) {
		while (state == 0)	//Off state
		{
			hour = RTC_TIME_GetHour();
			minute = RTC_TIME_GetMinute();
			if ((hour == 16) && (minute == 0))
				state = 1;
		}
		while (state == 1)	//Wait until light levels go below set threshold
		{
			Light_Get_ALS();
			Light_Calculate_Lux();
			if (!light)
			{
				state = 2;
				IR_Enable();
			}
		}
		while (state == 2)	//Wait until IR sensor interrupt is triggered, then flip the light switch
		{
			if (movement)
			{
				state = 3;
				light = true;
			}
		}
		while (state == 3)	//When the door moves, look for any movement and light in the room for the next ~2 seconds
		{											//If there is movement and lights are off or there is no movement and lights are on, flip light switch.
			Reset_Door();
			Delay(10000);
			prevdoor = Get_Door_State();
			Delay(10000);
			door = Get_Door_State();
			doorstate = door ^ prevdoor;
			if (doorstate)
			{
				Delay(8000000);
				IR_Enable();
				Delay(3000000);
				Light_Get_ALS();
				Light_Calculate_Lux();
				if (!movement && light)
				{
					Flip_Light_Switch();
					IR_Disable();
				}
				doorstate = false;
			}
		}
		Get_RTC_Calendar (strTime, strDate);
		LCD_DisplayString(strTime);
	}
}
