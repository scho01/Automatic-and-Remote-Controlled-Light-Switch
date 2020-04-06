#include "PWM.h"

void PWM_Init(void)
{
	Servo_PWM_Init();
	Ultrasonic_IC_Init();
	Ultrasonic_PWM_Init();
}

void Servo_PWM_Init(void) {
	RCC->AHB2ENR |= ((uint32_t)0x00000010U);
	RCC->APB2ENR |= ((uint32_t)0x00000800U);
	GPIOE->MODER &= ~GPIO_MODER_MODE8;
	GPIOE->MODER |= GPIO_MODER_MODE8_1;
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED8;
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT8;
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD8;
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL8;
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL8_0;
	
	TIM1->CR1 &= ~TIM_CR1_DIR;
	TIM1->PSC &= ~((uint16_t)0xFFFFU);
	TIM1->PSC |= ((uint16_t)0x004FU);
	TIM1->ARR &= ~((uint16_t)0xFFFFU);
	TIM1->ARR |= ((uint16_t)0x1770U);
	TIM1->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM1->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM1->CCMR1 |= TIM_CCMR1_OC1PE;
	TIM1->CCER &= ~TIM_CCER_CC1NP;
	TIM1->CCER |= TIM_CCER_CC1NE;
	TIM1->BDTR |= TIM_BDTR_MOE;
	TIM1->CCR1 &= ~((uint16_t)0xFFFFU);
	TIM1->CCR1 |= ((uint16_t)0x0500U);
	TIM1->CR1 |= ((uint32_t)0x00000001U);
}

void Flip_Light_Switch(void)
{
	TIM1->CCR1 |= (uint16_t)0x05A8U;
	for (int i = 0; i < 4000000; ++i) {}
	TIM1->CCR1 &= ~((uint16_t)0x00FFU);
}

uint32_t volatile currentValue = 0;
uint32_t volatile lastValue = 0;
uint32_t volatile overflowCount = 0;
uint32_t volatile timeInterval = 0;

void Ultrasonic_IC_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	GPIOA->MODER &= ~GPIO_MODER_MODE0;
	GPIOA->MODER |= GPIO_MODER_MODE0_1;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL0;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL0_1;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD0;
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM5EN;
	TIM5->CR1 |= TIM_CR1_ARPE;
	TIM5->ARR |= ((uint32_t)0x0000FFFFU);
	TIM5->PSC &= ~((uint16_t)0xFFFFU);
	TIM5->PSC |= ((uint16_t)0x0027U);
	TIM5->CCMR1 &= ~((uint32_t)0x00000002U);
	TIM5->CCMR1 |= ((uint32_t)0x00000001U);
	TIM5->CCER |= ((uint32_t)0x0000000BU);
	TIM5->DIER |= TIM_DIER_CC1DE;
	TIM5->DIER |= TIM_DIER_CC1IE;
	TIM5->DIER |= TIM_DIER_UIE;
	TIM5->EGR |= TIM_EGR_UG;
	TIM5->SR &= ~TIM_SR_UIF;
	TIM5->CR1 &= ~TIM_CR1_DIR;
	TIM5->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM5_IRQn);
	NVIC_SetPriority(TIM5_IRQn, 2);
}
void TIM5_IRQHandler(void) {
	if ((TIM5->SR & TIM_SR_UIF) != 0)
	{
		TIM5->SR &= ~TIM_SR_UIF;
		if (lastValue != 0)
		{
			overflowCount = 1;
			lastValue = (uint32_t)0xFFFF - lastValue;
		}
	}
	if ((TIM5->SR & TIM_SR_CC1IF) != 0)
	{
		if (lastValue == 0)
		{
			lastValue = TIM5->CCR1;
		}
		else
		{
			currentValue = TIM5->CCR1;
			if (overflowCount == 0)
				timeInterval = currentValue - lastValue;
			else
			{
				timeInterval = currentValue + lastValue;
				overflowCount = 0;
			}
			lastValue = 0;
		}
		TIM5->SR &= ~TIM_SR_CC1IF;
	}
}

void Ultrasonic_PWM_Init(void) {
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
	RCC->APB2ENR |= RCC_APB2ENR_TIM8EN;
	GPIOA->MODER &= ~GPIO_MODER_MODE5;
	GPIOA->MODER |= GPIO_MODER_MODE5_1;
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED5;
	GPIOA->OTYPER &= ~GPIO_OTYPER_OT5;
	GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD5;
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFSEL5;
	GPIOA->AFR[0] |= GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL5_1;

	TIM8->CR1 &= ~TIM_CR1_DIR;
	TIM8->PSC &= ~((uint16_t)0xFFFFU);
	TIM8->PSC |= ((uint16_t)0x004FU);
	TIM8->ARR |= ((uint16_t)0xFFFFU);
	TIM8->CCMR1 &= ~TIM_CCMR1_OC1M;
	TIM8->CCMR1 |= TIM_CCMR1_OC1M_1;
	TIM8->CCMR1 |= TIM_CCMR1_OC1M_2;
	TIM8->CCMR1 |= TIM_CCMR1_OC1PE;
	TIM8->CCER &= ~TIM_CCER_CC1NP;
	TIM8->CCER |= TIM_CCER_CC1NE;
	TIM8->BDTR |= TIM_BDTR_MOE;
	TIM8->CCR1 &= ~((uint16_t)0xFFFFU);
	TIM8->CCR1 |= ((uint16_t)0x000AU);
	TIM8->CR1 |= TIM_CR1_CEN;
}

bool Get_Door_State(void) // 0 = closed
{
	int door = timeInterval/5000;
	if (door > 3)
		Reset_Door();
	if (door == 0)
		return false;
	else
		return true;
}

void Reset_Door(void)
{
		currentValue = 0;
		lastValue = 0;
		overflowCount = 0;
		timeInterval = 0;
}
