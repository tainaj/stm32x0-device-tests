#include "interrupts_c.h"

/**
 * @brief Interrupt Service Handlers (definitions)
 *        (uncomment ISR according to settings in main.h)
 */

// TIMx ISRS
void TIM2_IRQ_handler(void) {
  // (1) Acknowledge TIM2 interrupt
  // (2) Set TIMx bit on flags.
  if (TIM2->SR & TIM_SR_UIF) {
    TIM2->SR &= ~(TIM_SR_UIF);  // (1)
    flags |= FL_TIMx;           // (2)
  }
}
#ifdef VVC_F0
void TIM3_IRQ_handler(void) {
  if (TIM3->SR & TIM_SR_UIF) {
    TIM3->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
void TIM14_IRQ_handler(void) {
  if (TIM14->SR & TIM_SR_UIF) {
    TIM14->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
void TIM15_IRQ_handler(void) {
  if (TIM15->SR & TIM_SR_UIF) {
    TIM15->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
void TIM16_IRQ_handler(void) {
  if (TIM16->SR & TIM_SR_UIF) {
    TIM16->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
void TIM17_IRQ_handler(void) {
  if (TIM17->SR & TIM_SR_UIF) {
    TIM17->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
#elif VVC_L0
void TIM21_IRQ_handler(void) {
  if (TIM21->SR & TIM_SR_UIF) {
    TIM21->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
void TIM22_IRQ_handler(void) {
  if (TIM22->SR & TIM_SR_UIF) {
    TIM22->SR &= ~(TIM_SR_UIF);
    flags |= FL_TIMx;
  }
}
#endif /* VVC_F0 or VVC_L0 */

// EXTIx ISRS
void EXTI4_15_IRQ_handler(void) {
  // Check that EXTI6 is the line that triggered.
  for (int i=4; i <= 15; i++) {
    if (EXTI->PR & (1 << i)) {
      flags |= FL_EXTIx;
      EXTI->PR |= (1 << i);
      break; // only one EXTI used for test 5
    }
  }
}

// DMA1_Channelz ISRS
void DMA1_chan1_IRQ_handler(void) {
  // Confirm Transfer Complete event
  if (DMA1->ISR & DMA_ISR_TCIF1) {
    DMA1->IFCR |= DMA_IFCR_CTCIF1;
    flags |= FL_DMAx;
  }

  // Acknowledge any DMA flag, including error flags
  DMA1->IFCR |= DMA_IFCR_CHTIF1
    | DMA_IFCR_CGIF1 | DMA_IFCR_CTEIF1;
}