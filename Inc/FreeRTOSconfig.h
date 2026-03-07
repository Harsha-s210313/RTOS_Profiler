#ifndef FREERTOSCONFIG_H_
#define FREERTOSCONFIG_H_

#define configUSE_MUTEXES								(1UL)
#define configUSE_PREEMPTION 							(1UL)

#define configCPU_CLOCK_HZ								(100000000UL)
#define configTICK_RATE_HZ								(1000UL)

#define configMAX_PRIORITIES							5
#define configMINIMAL_STACK_SIZE						128
#define configTOTAL_HEAP_SIZE							8192
#define configUSE_16_BIT_TICKS							0

#define configIDLE_SHOULD_YIELD							(1UL)
#define configUSE_TASK_NOTIFICATIONS					(1UL)

#define configPRIO_BITS									4
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

#define configKERNEL_INTERRUPT_PRIORITY					(configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))
#define configMAX_SYSCALL_INTERRUPT_PRIORITY			(configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

#define configUSE_IDLE_HOOK								0
#define configUSE_TICK_HOOK								0
#define configUSE_MALLOC_FAILED_HOOK					0
#define configCHECK_FOR_STACK_OVERFLOW					0
#define INCLUDE_vTaskDelay								1
#define INCLUDE_vTaskDelete								1
#define INCLUDE_vTaskSuspend							1
#define INCLUDE_vTaskPrioritySet						1
#define INCLUDE_uxTaskPriorityGet						1
#define INCLUDE_xTaskGetSchedulerState					1

#define vPortSVCHandler 								SVC_Handler
#define xPortPendSVHandler  							PendSV_Handler
#define xPortSysTickHandler  							SysTick_Handler
#endif /* FREERTOSCONFIG_H_ */
