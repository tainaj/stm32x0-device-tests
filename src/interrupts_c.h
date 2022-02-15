#ifndef _VVC_INTERRUPTS_C_H
#define _VVC_INTERRUPTS_C_H

#include "global.h"

/**
 * @brief Interrupt Service Handlers (declarations)
 *        (uncomment ISR according to settings in main.h)
 */

#ifdef VVC_F0
void TIM3_IRQ_handler(void);
void TIM14_IRQ_handler(void);
void TIM15_IRQ_handler(void);
void TIM16_IRQ_handler(void);
void TIM17_IRQ_handler(void);
#elif VVC_L0
void TIM21_IRQ_handler(void);
void TIM22_IRQ_handler(void);
#endif /* VVC_F0 or VVC_L0 */

void EXTI4_15_IRQ_handler(void);

void DMA1_chan1_IRQ_handler(void);

#endif