#include "delay.h"
#include "ti_msp_dl_config.h"

//����δ�ʱ��ʵ�ֵľ�ȷus��ʱ
void delay_us(unsigned long __us) {
	uint32_t ticks;
	uint32_t told, tnow, tcnt = 38;

	// ������Ҫ��ʱ���� = �ӳ�΢���� * ÿ΢���ʱ����
	ticks = __us * (CPUCLK_FREQ / 1000000);

	// ��ȡ��ǰ��SysTickֵ
	told = SysTick->VAL;

	while (1) {
		// �ظ�ˢ�»�ȡ��ǰ��SysTickֵ
		tnow = SysTick->VAL;
		if (tnow != told) {
            if (tnow < told) {
                tcnt += told - tnow;
            }else{
                tcnt += SysTick->LOAD - tnow + told;
			}
            told = tnow;
            // ����ﵽ����Ҫ��ʱ���������˳�ѭ��
            if (tcnt >= ticks) break;
        }
    }
}
//����δ�ʱ��ʵ�ֵľ�ȷms��ʱ
void delay_ms(unsigned long ms) { 
	delay_us(ms*1000);
}

void delay_1us(unsigned long __us) {
	delay_us(__us);
}

void delay_1ms(unsigned long ms) { 
	delay_ms(ms);
}