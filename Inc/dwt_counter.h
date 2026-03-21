
#ifndef DWT_COUNTER_H_
#define DWT_COUNTER_H_

#include<stdint.h>

void dwt_init(void);

uint32_t read_cycles(void);

uint32_t cast_to_usecs(uint32_t cycles);


#endif /* DWT_COUNTER_H_ */
