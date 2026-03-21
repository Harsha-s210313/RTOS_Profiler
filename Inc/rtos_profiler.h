
#ifndef RTOS_PROFILER_H_
#define RTOS_PROFILER_H_

#include<stdint.h>

typedef struct{
	char task_name[10];
	uint32_t cycle_in;
	uint32_t context_switch_count;
	uint32_t cycle_count;
	float CPU_percentage;
}Tasks_stats_t;

#define MAX_TASKS					5

void profiler_init(void);
void profiler_task_switched_in(void);
void profiler_task_switched_out(void);
void profiler_calc_stats(void);
void profiler_print_stats(void);


#endif /* RTOS_PROFILER_H_ */
