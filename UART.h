#ifndef __STM32L476G_DISCOVERY_UART_H
#define __STM32L476G_DISCOVERY_UART_H

//Bluetooth Module
//Uses GPIO PA2 (TX) and GPIO PA3 (RX)


#include "stm32l476xx.h"

extern uint8_t state;

void USART_Init(void);
void USART2_IRQHandler(void);
void Delay(uint32_t us);

#endif
