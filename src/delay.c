#include "delay.h"

/**
 * @brief Delay module for busy waiting
 *        Peripherals used:
 *        - SysTick
 *        - TIM2
 */

/**
 * ----------------------------------------------------
 *                 Private variables.
 * ----------------------------------------------------
 */

static volatile uint32_t _millis = 0;

/**
 * @brief Interrupt service handlers (ISR)
 */

void SysTick_handler(void) {
    _millis++;
}

/**
 * @brief ---------------------------------------------
 *                Public functions (Delay API)
 * ----------------------------------------------------
 */

/**
 * @brief Initialize SysTick clock. Ticks milliseconds.
 * 
 * @param _core_clock_hz 
 */
void systick_init(uint32_t _core_clock_hz) {
    SysTick_Config(_core_clock_hz / 1000);
    NVIC_SetPriority(SysTick_IRQn, 0);
}

/**
 * @brief Initialize timer. Ticks microseconds.
 * 
 */
void timer_us_init(uint32_t _core_clock_hz) {
    /* (1) Configure PSC time to 1us prescaler */
    /* (2) Configure ARR time to 16-bit counter */

    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->CR1 &= ~TIM_CR1_CEN;
    TIM2->PSC = _core_clock_hz / 1000000;   /* (1) */
    TIM2->ARR = 0xffff;                     /* (2) */

    TIM2->CR1 |= TIM_CR1_CEN;
}

uint32_t millis(void) {
    return _millis;
}

/**
 * @brief Return time in micros (mod _max_micros)
 */
uint16_t micros(void) {
    return (uint16_t)(TIM2->CNT);
}

/**
 * @brief Delay time (d) with given micro time
 * 
 * @param delay   delay in us (keep below 65.535 ms)
 */
void micro_wait(uint16_t delay) {
    uint16_t time_start = micros();
    while ((uint16_t)(micros() - time_start) < delay + 1);
}

/**
 * @brief Delay time with given milli time
 * 
 * @param delay (keep below 46 days)
 *        
 */
void milli_wait(uint32_t delay) {
    uint32_t time_start = millis();
    while ((uint32_t)(millis() - time_start) < delay + 1);
}

/**
 * @brief Compatibility API for generic us delay
 * 
 * @param delay (keep below 1 hour)
 *        - delay <= 60000 us -> micro_wait()
 *        - delay >  60000 us -> milli_wait()
 */
void delay(uint32_t delay) {
    if (delay < 60000) {
        micro_wait((uint16_t)(delay));
    }
    else {
        milli_wait((uint32_t)(delay / 1000));
    }
}