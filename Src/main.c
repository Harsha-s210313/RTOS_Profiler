#include<stdio.h>
#include<stm32f411xe.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "FreeRTOSconfig.h"
#include "dwt_counter.h"
#include "rtos_profiler.h"
#include "uart.h"

#define USART2EN    (1U<<17)
#define USART_TE    (1U<<3)
#define USART_UE    (1U<<13)
#define USART_TXE   (1U<<7)

QueueHandle_t xqueue1;
SemaphoreHandle_t xButtonSemaphore;
SemaphoreHandle_t xUARTmutex;


void vTask1(void *pvParameter);
void vTask2(void *pvParameter);
void vTask3(void *pvParameter);
void vTask4(void *pvParameter);
void vStatsTask(void *pvParameter);
void EXTI15_10_IRQHandler(void);
int main(){
	xqueue1 =  xQueueCreate(5, sizeof(int));
	profiler_init();
	dwt_init();
	RCC->AHB1ENR |= (5U<<0);
	RCC->APB2ENR |= (1U<<14);

	GPIOA->MODER &= ~(3U<<10);
	GPIOA->MODER |= (1U<<10);

	GPIOC->MODER &= ~(3U<<26);
	SYSCFG->EXTICR[3] &= ~(15U<<4);
	SYSCFG->EXTICR[3] |= (2U<<4);

	EXTI->IMR |= (1U<<13);
	EXTI->FTSR |= (1U<<13);

	NVIC_EnableIRQ(EXTI15_10_IRQn);

	xButtonSemaphore = xSemaphoreCreateBinary();
	xUARTmutex = xSemaphoreCreateMutex();

	uart_init();

	xTaskCreate(vTask1, "Task1",configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTask2, "Task2",configMINIMAL_STACK_SIZE, NULL, 1, NULL);
	xTaskCreate(vTask3, "Task3",configMINIMAL_STACK_SIZE, NULL, 2, NULL);
	xTaskCreate(vTask4, "Task4",configMINIMAL_STACK_SIZE, NULL, 3, NULL);
	xTaskCreate(vStatsTask, "Stats", 256, NULL, 4, NULL);


	vTaskStartScheduler();

	while(1);
}

void vTask1(void *pvParameter){
	int k =0;
	for(;;){
		k++;
		xQueueSend(xqueue1, &k, portMAX_DELAY);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void vTask2(void *pvParameter){
	int recieved_value = 0;
	for(;;){
		BaseType_t q = xQueueReceive(xqueue1, &recieved_value, portMAX_DELAY);
		if(q == pdTRUE){
			char buffer[16];
			sprintf(buffer, "%d", recieved_value);
			xSemaphoreTake(xUARTmutex, portMAX_DELAY);
			uart_send_string(buffer);
			xSemaphoreGive(xUARTmutex);
			vTaskDelay(pdMS_TO_TICKS(1000));
		}
		else{
			vTaskDelay(pdMS_TO_TICKS(500));
		}
	}
}
void vTask3(void *pvParameter){
	for(;;){
		xSemaphoreTake(xUARTmutex, portMAX_DELAY);
		uart_send_string("Hello PC. THIS TASK HAS HIGHER PRIORITY(2). SO THIS MUST BE EXECTUED WITH IMMEDIATE EFFECT.\n\r");
		xSemaphoreGive(xUARTmutex);
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}

void vTask4(void *pvParameter){
	for(;;){
		xSemaphoreTake(xButtonSemaphore, portMAX_DELAY);
		GPIOA->ODR ^= (1U<<5);
	}
}
void vStatsTask(void *pvParameter){
	for(;;){
		profiler_calc_stats();
		profiler_print_stats();
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void EXTI15_10_IRQHandler(void){
	xSemaphoreGiveFromISR(xButtonSemaphore, NULL);
	EXTI->PR|=(1U<<13);
}
