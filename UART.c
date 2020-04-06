#include "UART.h"
#include "PWM.h"
#include "Light.h"

uint8_t state = 0;

void USART_Init(void) {
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
  RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
  RCC->CCIPR |= RCC_CCIPR_USART2SEL_0;
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
  GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
  GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);
  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);
  GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL2_0 | GPIO_AFRL_AFSEL2_1 | GPIO_AFRL_AFSEL2_2 | GPIO_AFRL_AFSEL3_0 | GPIO_AFRL_AFSEL3_1 | GPIO_AFRL_AFSEL3_2);    
  GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED3;
  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT3);
  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD3);
  GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0 | GPIO_PUPDR_PUPD3_0;

  USART2->CR1 &= ~USART_CR1_UE;
  USART2->CR1 &= ~(USART_CR1_M | USART_CR1_OVER8);
  USART2->CR2 &= ~USART_CR2_STOP;
  USART2->BRR &= ~((uint16_t)0xFFFFU);
  USART2->BRR |= ((uint16_t)0x208DU);
  USART2->CR1 |= USART_CR1_RE | USART_CR1_RXNEIE;
  USART2->CR1 |= USART_CR1_UE;
  NVIC_EnableIRQ(USART2_IRQn);
  NVIC_SetPriority(USART2_IRQn, 0);
}

void Delay(uint32_t us) {
  uint32_t time = 100*us/7;
  while(--time);
}

void USART2_IRQHandler(void)
{
  uint32_t SR = USART2->ISR;
  if (SR & USART_ISR_RXNE)
  {
		char buff = ((uint8_t)(USART2->RDR & 0xFF));
		Light_Get_ALS();
		Light_Calculate_Lux();
    if (buff == 'a')
    {
			if (!light)
			{
				Flip_Light_Switch();
			}
			state = 3;
    }
		else if (buff == 'b')
		{
			if (light)
			{
				Flip_Light_Switch();
			}
			state = 0;
		}
  }
}
