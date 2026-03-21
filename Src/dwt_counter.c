
#include<dwt_counter.h>
#include<stm32f411xe.h>

void dwt_init(void){
	CoreDebug->DEMCR |= (1U<<24);
	DWT->CYCCNT = 0;
	DWT->CTRL |= (1U<<0);
}
uint32_t read_cycles(void){
	return DWT->CYCCNT;
}
uint32_t cast_to_usecs(uint32_t cycles){
	return cycles/100;
}
