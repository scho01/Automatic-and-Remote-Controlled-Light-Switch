#ifndef __STM32L476G_DISCOVERY_Light_H
#define __STM32L476G_DISCOVERY_Light_H

//Light Sensor
//Uses GPIO PB6 (SCL) and GPIO PB7 (SDA)

#include "stm32l476xx.h"
#include "stdbool.h"

extern bool light;

bool Light_Init(void);
void Light_Enable(void);
void Light_Disable(void);
void Light_Set_Gain(uint8_t gain1);
void Light_Set_Time(uint8_t integ1);
void Light_Get_ALS(void);
void Light_Calculate_Lux(void);


#endif
