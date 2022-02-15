#ifndef _VVC_MY_TIMER_H
#define _VVC_MY_TIMER_H

#include "global.h"

// Define for disabling a channel
#define TIM_DISABLE 0xffff

/* Timer Peripherals */
void reset_timer(TIM_TypeDef *TIMx);
void start_timer(TIM_TypeDef *TIMx, uint16_t arr, uint16_t div);
void start_timer_psc_timer(TIM_TypeDef *TIMm, TIM_TypeDef *TIMs, uint16_t arr_m, uint16_t arr_s, uint16_t div_m);
void master_timer(TIM_TypeDef *TIMm, uint8_t mms);
void slave_timer(TIM_TypeDef *TIMs, uint8_t sms, uint8_t ts);
uint8_t ccr_count(TIM_TypeDef *TIMx);
void ccr_timer(TIM_TypeDef *TIMx, uint16_t ccr1, uint16_t ccr2, uint16_t ccr3, uint16_t ccr4);

#endif