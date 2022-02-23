#ifndef _VVC_MAIN_H
#define _VVC_MAIN_H

#include "global.h"
#include "my_timer.h"
#include "LiquidCrystal_I2C.h"
#include "interrupts_c.h"
#include "button.h"

// Define test number (1 to 7)
#define TEST_NO     (6)

// Define GPIOx pin mappings for I2Cy
// (supports pins for x = GPIOA, GPIOB)
// (supports y = I2C1, I2C2)
// (I2Cy alt function is always AF1)
// (...except L0: I2C1(SCL,SDA) = (PB8,PB9) -> AF4, adjust manually in main.c)
// (check your device datasheet)
#define I2C_X       (I2C1)
#define I2C_X_GPIOx (GPIOB)
#define PX_SCL      (6)
#define PX_SDA      (7)

#if ((TEST_NO == 2) || (TEST_NO == 3) || (TEST_NO == 4) || (TEST_NO == 5) || (TEST_NO == 6))
// Test 2, 3, 4: Define GPIOx pin mappings for ... [1 LED output]
// (supports pins for GPIOA, GPIOB, GPIOC)
// (check your device datasheet)
#define LED_GPIOx   (GPIOB)
#define LED_PIN_1   (3)
#define LED_PIN_2   (4) // disabled if test 2, 3, 4, 5
#define LED_PIN_3   (5) // disabled if test 2, 3, 4, 5

#endif /* LED_GPIOx */

#if ((TEST_NO == 5) || (TEST_NO == 6) || (TEST_NO == 7))
// Test 5, 6: Define GPIOx pin mappings for ... [1 input]
// (supports pins for GPIOA, GPIOB, GPIOC)
// (check your device datasheet)
#define INPUT_GPIOx     (GPIOA)
#define INPUT_1         (3)
#define INPUT_2         (4) // disabled if test 5
#define INPUT_3         (5) // disabled if test 5, 7

#endif /* INPUT_GPIOx */

#if ((TEST_NO == 2) || (TEST_NO == 3) || (TEST_NO == 4) || (TEST_NO == 6) || (TEST_NO == 7))
// Test 2, 3, 4: Define TIMx peripherals (slave TIM if test 4)
// (supports timer X = 3,14,15,16,17,21,22)
// (check your device datasheet)
// (remember to uncomment the TIMx_IRQ_handler in interrupts_c.c and interrupts_c.h!)
#define TIM_X           (TIM22)
#define TIM_X_IRQn      (TIM22_IRQn) // disabled if test 7

#endif /* TIM_X */

#if ((TEST_NO == 3))
// Test 3: Define GPIOx pin mappings for TIM_X channel outputs
// (supports pins for x = GPIOA, GPIOB)
// (...except L0: TIM22(CH1,CH2) = (PA6,PA7) -> AF5, does not work)
// (supports CCR alt function no. y = [0,7])
// (check your device datasheet)
#define TIM_X_CCR_GPIOx     (GPIOA)
#define TIM_X_CCR_AFy       (0x5)
#define TIM_X_CCR_PIN_1     (9)
#define TIM_X_CCR_PIN_2     (10)
#define TIM_X_CCR_PIN_3     (0) // null
#define TIM_X_CCR_PIN_4     (1) // null
#endif /* TIM_X_CCR_GPIOx */

#if ((TEST_NO == 4))
// Test 4: Define TIMy peripherals (master TIM)
// (supports master timer Y = 3,14,15,16,17,...)
// (supports slave timer X TS = 000 to 011)
// (check your device datasheet)
// (remember to uncomment the TIMx_IRQ_handler in interrupts_c.c and interrupts_c.h!)
#define TIM_Y           (TIM21)
#define TIM_X_TS        (0x0)

#endif /* TIM_Y */

#if ((TEST_NO == 5))
// Test 5: Define EXTI IRQn for EXTIx (INPUT_1 = x)
// (supports x = [0,1] -> EXTI0_1_IRQn)
// (supports x = [2,3] -> EXTI2_3_IRQn)
// (supports x = [4,15] -> EXTI4_15_IRQn)
// (check your device datasheet)
#define EXTI_X_IRQn        (EXTI2_3_IRQn)

#endif /* EXTI_X_IRQn */

#if ((TEST_NO == 7))
// Test 7: Define 2 ADC channels
// (supports z = [0,15] -> ADC_INz -> GPIO pin)
// (enable corresponding `INPUT_GPIOx`, two INPUT pins)
// (F0: supports TRGO = (0x3->TIM3_TRGO),  (0x4->TIM15_TRGO)
// (L0: supports TRGO = (0x4->TIM22_TRGO))
// (enable corresponding `TIM_X`)
// (check your device datasheet)
#define ADC1_INz_1      (0) // corresponds to `INPUT_1`
#define ADC1_INz_2      (1) // corresponds to `INPUT_2`
#define ADC1_TRGO       (0x4)

#endif /* EXTI_X_IRQn */

#endif
