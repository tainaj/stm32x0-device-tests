#include "my_timer.h"

/**
 * @brief Custom timer setup module
 *        Any timer TIMx, except TIM2 (reserved for delay.c)
 */

/**
 * @brief This method enables and resets the timer peripheral,
 *           also turns off the timer's 'counter'.
 * 
 * @param TIMx Timer peripheral
 */
void reset_timer(TIM_TypeDef *TIMx) {
    // Start by reseting the timer. (This is where a HAL can help)
    /* (1) Enable the TIMx clock on RCC. */
    /* (2) Make sure the timer's 'counter' is off. */
    /* (3) Reset the peripheral. */
    if (TIMx == TIM2) {
        RCC->APB1ENR  |=  (RCC_APB1ENR_TIM2EN);     /* (1) */
        TIM2->CR1     &= ~(TIM_CR1_CEN);            /* (2) */
        RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM2RST);   /* (3) */
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM2RST);
    }
    #ifdef VVC_F0
    else if (TIMx == TIM3) {
        RCC->APB1ENR  |=  (RCC_APB1ENR_TIM3EN);
        TIM3->CR1     &= ~(TIM_CR1_CEN);
        RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM3RST);
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM3RST);
    }
    else if (TIMx == TIM14) {
        RCC->APB1ENR  |=  (RCC_APB1ENR_TIM14EN);
        TIM14->CR1    &= ~(TIM_CR1_CEN);
        RCC->APB1RSTR |=  (RCC_APB1RSTR_TIM14RST);
        RCC->APB1RSTR &= ~(RCC_APB1RSTR_TIM14RST);
    }
    else if (TIMx == TIM15) {
        RCC->APB2ENR  |=  (RCC_APB2ENR_TIM15EN);
        TIM15->CR1    &= ~(TIM_CR1_CEN);
        RCC->APB2RSTR |=  (RCC_APB2RSTR_TIM15RST);
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM15RST);
    }
    else if (TIMx == TIM16) {
        RCC->APB2ENR  |=  (RCC_APB2ENR_TIM16EN);
        TIM16->CR1    &= ~(TIM_CR1_CEN);
        RCC->APB2RSTR |=  (RCC_APB2RSTR_TIM16RST);
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM16RST);
    }
    else if (TIMx == TIM17) {
        RCC->APB2ENR  |=  (RCC_APB2ENR_TIM17EN);
        TIM17->CR1    &= ~(TIM_CR1_CEN);
        RCC->APB2RSTR |=  (RCC_APB2RSTR_TIM17RST);
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM17RST);
    }
    #elif VVC_L0
    else if (TIMx == TIM21) {
        RCC->APB2ENR  |=  (RCC_APB2ENR_TIM21EN);
        TIM21->CR1    &= ~(TIM_CR1_CEN);
        RCC->APB2RSTR |=  (RCC_APB2RSTR_TIM21RST);
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM21RST);
    }
    else if (TIMx == TIM22) {
        RCC->APB2ENR  |=  (RCC_APB2ENR_TIM22EN);
        TIM22->CR1    &= ~(TIM_CR1_CEN);
        RCC->APB2RSTR |=  (RCC_APB2RSTR_TIM22RST);
        RCC->APB2RSTR &= ~(RCC_APB2RSTR_TIM22RST);
    }
    #endif
}

/**
 * @brief This method starts a new 'general-purpose' timer,
 * which triggers an 'UPDATE' interrupt every time it triggers.
 *        Note: Call reset_timer() before calling this method.
 * 
 * @param TIMx  Timer peripheral
 * @param arr   Number of ticks per update (default: milliseconds)
 * @param div   Prescaler default (core_clock_hz / 1000) is 
 *              reduced by this factor 'div'
 *              (Useful for ms = 1, or core_clock_hz > 65.536 MHz)
 */
void start_timer(TIM_TypeDef *TIMx, uint16_t arr, uint16_t div) {
    // Set the timer prescaler/autoreload timing registers.
    // (These are 16-bit timers, so this won't work with >65MHz.)
    TIMx->PSC   = ((core_clock_hz / 1000) / div) - 1;
    TIMx->ARR   = (arr * div) - 1;
    // Send an update event to reset the timer and apply settings.
    TIMx->EGR  |= TIM_EGR_UG;
    // Enable the hardware interrupt.
    TIMx->DIER |= TIM_DIER_UIE;
    // Enable the timer.
    TIMx->CR1  |= TIM_CR1_CEN;
}

/**
 * @brief This method starts a clock prescaled by the other
 * 
 * @param TIMm Timer peripheral (master)
 * @param TIMs Timer peripheral (slave)
 * @param arr_m Number of ticks per master update (default: milliseconds)
 * @param arr_s Number of master updates per slave update
 * @param div_m Prescaler default (core_clock_hz / 1000) is 
 *              reduced by this factor 'div'
 *              (Useful for arr_m = 1, or core_clock_hz > 65.536 MHz)
 */
void start_timer_psc_timer(TIM_TypeDef *TIMm, TIM_TypeDef *TIMs, uint16_t arr_m, uint16_t arr_s, uint16_t div_m) {
    // Set the timer prescaler/autoreload timing registers.
    // (These are 16-bit timers, so this won't work with >65MHz.)
    TIMm->PSC   = ((core_clock_hz / 1000) / div_m) - 1;
    TIMm->ARR   = (arr_m * div_m) - 1;
    TIMs->ARR   = arr_s - 1;
    // Send an update event to reset the slave timer and apply settings.
    TIMs->EGR  |= TIM_EGR_UG;
    // Enable the hardware interrupt.
    TIMs->DIER |= TIM_DIER_UIE;
    // Enable the timers.
    TIMs->CR1  |= TIM_CR1_CEN;
    TIMm->CR1  |= TIM_CR1_CEN;
}

/**
 * @brief This method configures a timer in master mode
 * 
 * @param TIMm Timer peripheral (master)
 * @param mms  Master mode selection [2:0]
 */
void master_timer(TIM_TypeDef *TIMm, uint8_t mms) {
    // configure TIMm in master mode
    TIMm->CR2 &= ~(TIM_CR2_MMS);
    TIMm->CR2 |= (mms & 0x1) ? TIM_CR2_MMS_0 : 0;
    TIMm->CR2 |= (mms & 0x2) ? TIM_CR2_MMS_1 : 0;
    TIMm->CR2 |= (mms & 0x4) ? TIM_CR2_MMS_2 : 0;
}

/**
 * @brief This method configures a timer in master mode
 * 
 * @param TIMs Timer peripheral (slave)
 * @param sms  Slave mode selection [2:0]
 * @param ts   Trigger selection [2:0]
 */
void slave_timer(TIM_TypeDef *TIMs, uint8_t sms, uint8_t ts) {
    // configure TIMs in slave mode selection
    TIMs->SMCR &= ~(TIM_SMCR_SMS);
    TIMs->SMCR |= (sms & 0x1) ? TIM_SMCR_SMS_0 : 0;
    TIMs->SMCR |= (sms & 0x2) ? TIM_SMCR_SMS_1 : 0;
    TIMs->SMCR |= (sms & 0x4) ? TIM_SMCR_SMS_2 : 0;
    // configure TIMs in trigger selection
    TIMs->SMCR &= ~(TIM_SMCR_TS);
    TIMs->SMCR |= (ts & 0x1) ? TIM_SMCR_TS_0 : 0;
    TIMs->SMCR |= (ts & 0x2) ? TIM_SMCR_TS_1 : 0;
    TIMs->SMCR |= (ts & 0x4) ? TIM_SMCR_TS_2 : 0;
}

/**
 * @brief This method checks how many channels TIMx has
 * 
 * @param TIMx Timer peripheral.
 * @return uint8_t 
 */
uint8_t ccr_count(TIM_TypeDef *TIMx) {
    uint8_t ch_num;
    if (TIMx == TIM2) {
        ch_num = 4;
    }
    #ifdef VVC_F0
    else if (TIMx == TIM3) {
        ch_num = 4;
    }
    else if (TIMx == TIM14) {
        ch_num = 1;
    }
    else if (TIMx == TIM15) {
        ch_num = 2;
    }
    else if (TIMx == TIM16) {
        ch_num = 1;
    }
    else if (TIMx == TIM17) {
        ch_num = 1;
    }
    #elif VVC_L0
    else if (TIMx == TIM21) {
        ch_num = 2;
    }
    else if (TIMx == TIM22) {
        ch_num = 2;
    }
    #endif

    return ch_num;
}

/**
 * @brief This method configures channel interrupts
 * 
 * @param TIMx Timer peripheral
 * @param ccr1 CCR1 <= ARR
 * @param ccr2 CCR2 <= ARR
 * @param ccr3 CCR3 <= ARR
 * @param ccr4 CCR4 <= ARR
 */
void ccr_timer(TIM_TypeDef *TIMx, uint16_t ccr1, uint16_t ccr2, uint16_t ccr3, uint16_t ccr4) {
    /* (1) Set CCRx values (or TIM_DISABLE to disable) */
    /* (2) Set all channels to toggle LED (OCxM = 011) */
    /* (3) Enable OCx outputs */
    /* (4) (TIM15,TIM16,TIM17 only) Enable OC outputs for real */
    uint8_t ch_num = ccr_count(TIMx);

    if (ch_num >= 1) {
        TIMx->CCR1  =   (ccr1 == TIM_DISABLE) ? 0 : ccr1-1;                                 /* (1) */
        TIMx->CCMR1 &= ~(TIM_CCMR1_OC1M);
        TIMx->CCMR1 |=  (ccr1 == TIM_DISABLE) ? 0 : TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;    /* (2) */
        TIMx->CCER  &= ~(TIM_CCER_CC1E);
        TIMx->CCER  |=  (ccr1 == TIM_DISABLE) ? 0 : TIM_CCER_CC1E;                          /* (3) */
    }
    if (ch_num >= 2) {
        TIMx->CCR2  =   (ccr2 == TIM_DISABLE) ? 0 : ccr2-1;
        TIMx->CCMR1 &= ~(TIM_CCMR1_OC2M);
        TIMx->CCMR1 |=  (ccr2 == TIM_DISABLE) ? 0 : TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1;
        TIMx->CCER  &= ~(TIM_CCER_CC2E);
        TIMx->CCER  |=  (ccr2 == TIM_DISABLE) ? 0 : TIM_CCER_CC2E;
    }
    if (ch_num >= 3) {
        TIMx->CCR3  =   (ccr3 == TIM_DISABLE) ? 0 : ccr3-1;
        TIMx->CCMR2 &= ~(TIM_CCMR2_OC3M);
        TIMx->CCMR2 |=  (ccr3 == TIM_DISABLE) ? 0 : TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1;
        TIMx->CCER  &= ~(TIM_CCER_CC3E);
        TIMx->CCER  |=  (ccr3 == TIM_DISABLE) ? 0 : TIM_CCER_CC3E;
    }
    if (ch_num >= 4) {
        TIMx->CCR4  =   (ccr4 == TIM_DISABLE) ? 0 : ccr4-1;
        TIMx->CCMR2 &= ~(TIM_CCMR2_OC4M);
        TIMx->CCMR2 |=  (ccr4 == TIM_DISABLE) ? 0 : TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1;
        TIMx->CCER  &= ~(TIM_CCER_CC4E);
        TIMx->CCER  |=  (ccr4 == TIM_DISABLE) ? 0 : TIM_CCER_CC4E;
    }

    #ifdef VVC_F0
    if (TIMx == TIM15) {
        TIMx->BDTR |= TIM_BDTR_MOE;
    }
    if (TIMx == TIM16) {
        TIMx->BDTR |= TIM_BDTR_MOE;
    }
    if (TIMx == TIM17) {
        TIMx->BDTR |= TIM_BDTR_MOE;
    }
    #endif
}