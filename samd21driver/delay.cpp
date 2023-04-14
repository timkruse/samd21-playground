#include "delay.h"


volatile uint32_t g_ms_ticks = 0;

void delay_ms(uint32_t ul_dly_ticks) {
	uint32_t ul_cur_ticks = g_ms_ticks;
	while ((g_ms_ticks - ul_cur_ticks) < ul_dly_ticks) {}
}