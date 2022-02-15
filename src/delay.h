#ifndef _VVC_DELAY_H
#define _VVC_DELAY_H

#include <stdint.h>
#include "global.h"

void systick_init(uint32_t);
void timer_us_init(uint32_t);
uint16_t micros(void);
uint32_t millis(void);
void micro_wait(uint16_t delay);
void milli_wait(uint32_t delay);

void delay(uint32_t);

#endif