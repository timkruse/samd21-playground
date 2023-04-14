#pragma once

#ifndef DRIVER_DELAY_H
#define DRIVER_DELAY_H

#include <stdint.h>

extern volatile uint32_t g_ms_ticks;

/**
 * \brief Delay number of tick Systicks (happens every 1 ms).
 */
extern void delay_ms(uint32_t ul_dly_ticks);

#endif // DRIVER_DELAY_H