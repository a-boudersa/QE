#include "stm32f4xx.h"                  // Device header
#include "arm_math.h"


void initdisp(void);
//void updatedisp(uint16_t flag);
void updategraph(q15_t Vtrms,q15_t THDu,q15_t A9,q15_t THDd,uint16_t i);
