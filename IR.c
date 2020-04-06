#include "IR.h"
#include "Light.h"
#include "PWM.h"

bool movement = false;

void IR_Init(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODE1);
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD1);
  GPIOA->PUPDR |= GPIO_PUPDR_PUPD1_1;
  SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI1;
  SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI1_PA;
  EXTI->RTSR1 |= EXTI_RTSR1_RT1;
  EXTI->FTSR1 |= EXTI_FTSR1_FT1;
  EXTI->PR1 |= EXTI_PR1_PIF1;
	movement = false;
}

void EXTI1_IRQHandler(void)
{
  EXTI->PR1 |= EXTI_PR1_PIF1;
	movement = true;
	Light_Get_ALS();
	Light_Calculate_Lux();
	if (!light)
	{
		Flip_Light_Switch();
	}
	NVIC_DisableIRQ(EXTI1_IRQn);
  EXTI->IMR1 &= ~(EXTI_IMR1_IM1);
}


void IR_Enable(void)
{
	movement = false;
  EXTI->IMR1 |= EXTI_IMR1_IM1;
  NVIC_EnableIRQ(EXTI1_IRQn);
  NVIC_SetPriority(EXTI1_IRQn, 1);
  EXTI->PR1 |= EXTI_PR1_PIF1;
}

void IR_Disable(void)
{
	NVIC_DisableIRQ(EXTI1_IRQn);
  EXTI->IMR1 &= ~(EXTI_IMR1_IM1);
}
