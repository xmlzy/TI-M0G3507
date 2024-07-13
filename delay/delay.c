#include "delay.h"
#include "ti_msp_dl_config.h"

//搭配滴答定时器实现的精确us延时
void delay_us(unsigned long __us) {
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 38;

	// 计算需要的时钟数 = 延迟微秒数 * 每微秒的时钟数
	ticks = __us * (CPUCLK_FREQ / 1000000);

	// 获取当前的SysTick值
	told = SysTick->VAL;

	while (1) {
		// 重复刷新获取当前的SysTick值
		tnow = SysTick->VAL;
		if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            }else{
                tcnt += SysTick->LOAD - tnow + told;
			}
            told = tnow;
            // 如果达到了需要的时钟数，就退出循环
            if (tcnt >= ticks) break;
        }
    }
}
//搭配滴答定时器实现的精确ms延时
void delay_ms(unsigned long ms) { 
	delay_us(ms*1000);
}

void delay_1us(unsigned long __us) {
	delay_us(__us);
}

void delay_1ms(unsigned long ms) { 
	delay_ms(ms);
}