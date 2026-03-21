
#include<stm32f411xe.h>
#include "uart.h"

void uart_init(void){
	RCC->APB1ENR |= (1U<<17);
	RCC->AHB1ENR |= (1U<<0);
	//PA2 for USART_TX
	GPIOA->MODER &= ~(3U<<4);
	GPIOA->MODER |= (2U<<4);

	GPIOA->AFR[0] &= ~(15U<<8);
	GPIOA->AFR[0] |= (7U<<8);

	USART2->BRR = 0x8AE;

	USART2->CR1 |= (1U<<3);
	USART2->CR1 |= (1U<<13);
}

void uart_send_string(const char *s){
	while (*s != '\0') {
	        while (!(USART2->SR & (1U << 7)));
	        USART2->DR = (uint8_t)(*s);
	        s++;
	    }
}
