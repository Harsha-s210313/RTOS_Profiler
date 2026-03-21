
#include<stdint.h>
#include<stdio.h>
#include<string.h>
#include "rtos_profiler.h"
#include "FreeRTOS.h"
#include "dwt_counter.h"
#include "uart.h"
#include "task.h"

Tasks_stats_t task_mask[MAX_TASKS];

uint32_t total_cycles;

void profiler_init(void){
	memset(&task_mask, 0, sizeof(task_mask));
	total_cycles = 0;
}

void profiler_task_switched_in(void){
	const char *current_task = pcTaskGetTaskName(NULL);
	for (int i = 0; i< MAX_TASKS;i++){
		if(strcmp(task_mask[i].task_name,current_task ) == 0){
			task_mask[i].cycle_in = read_cycles();
			break;
		}

		else if(task_mask[i].task_name[0] == '\0'){
		            strncpy(task_mask[i].task_name, current_task, 9);
		            task_mask[i].task_name[9] = '\0';
		            task_mask[i].cycle_in = read_cycles();
		            break;
		}
	}
}
void profiler_task_switched_out(void){
	const char *current_task = pcTaskGetTaskName(NULL);
	for (int i = 0 ; i< MAX_TASKS; i++){
		if(strcmp(current_task,task_mask[i].task_name) == 0){
			uint32_t elapsed_time = read_cycles() - task_mask[i].cycle_in;
			task_mask[i].cycle_count += elapsed_time;
			task_mask[i].context_switch_count++;
			total_cycles += elapsed_time;
			break;
		}
	}
}
void profiler_calc_stats(void){
	for (int i = 0; i< MAX_TASKS; i++){
		task_mask[i].CPU_percentage = ((float)task_mask[i].cycle_count / total_cycles) * 100 ;
	}
}

void profiler_print_stats(void){
	    char buf[64];
	    for(int i = 0; i < MAX_TASKS; i++){
	        if(task_mask[i].task_name[0] == '\0'){
	            continue;  // skip empty slots
	        }
	        sprintf(buf, "{\"task\":\"%s\",\"cpu\":%.2f,\"switches\":%lu}\n",
	                task_mask[i].task_name,
	                task_mask[i].CPU_percentage,
	                task_mask[i].context_switch_count);
	        uart_send_string(buf);
	    }
}
