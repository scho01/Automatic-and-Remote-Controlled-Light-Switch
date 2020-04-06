#ifndef __STM32L476G_DISCOVERY_PWM_H
#define __STM32L476G_DISCOVERY_PWM_H

//Servo and Ultrasonic Sensor
//Uses GPIO PE8 (Servo PWM), GPIO PA0 (Ultrasonic Echo Input), and GPIO PA5 (Ultrasonic Trigger)

#include "stm32l476xx.h"
#include "stdbool.h"

void PWM_Init(void);
void Servo_PWM_Init(void);
void Flip_Light_Switch(void);
void Ultrasonic_IC_Init(void);
void Ultrasonic_PWM_Init(void);
bool Get_Door_State(void);
void Reset_Door(void);

#endif
