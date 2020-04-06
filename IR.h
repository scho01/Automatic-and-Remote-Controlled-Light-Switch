#ifndef __STM32L476G_DISCOVERY_IR_H
#define __STM32L476G_DISCOVERY_IR_H

//IR Sensor
//Uses GPIO PA1 (Input)

#include "stm32l476xx.h"
#include "stdbool.h"

extern bool movement;

void IR_Init(void);
void IR_Enable(void);
void IR_Disable(void);

#endif
