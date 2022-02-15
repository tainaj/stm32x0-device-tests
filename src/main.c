#include "main.h"

/**
 * @brief Implementation of magic function itoa()
 *        - swap(), reverse(), itoa()
 */
// Function to swap two numbers
void swap(char *x, char *y) {
  char t = *x; *x = *y; *y = t;
}

// Function to reverse `buffer[i...j]`
char* reverse(char *buffer, int i, int j) {
  while (i < j) {
    swap(&buffer[i++], &buffer[j--]);
  }
  return buffer;
}

// Iterative function to implement `itoa()` function in C
char* itoa(uint32_t value, char* buffer, int base) {
  // invalid input
  if (base < 2 || base > 32) {
    return buffer;
  }

  // consider the absolute value of the number (always positive)
  uint32_t n = value;

  int i = 0;
  while (n) {
    int r = n % base;

    if (r >= 10) {
      buffer[i++] = 65 + (r - 10);
    }
    else {
      buffer[i++] = 48 + r;
    }

    n = n / base;
  }

  // if the number is 0
  if (i == 0) {
    buffer[i++] = '0';
  }

  // Value cannot be negative (skip)

  buffer[i] = '\0'; // null terminate string

  // reverse the string and return it
  return reverse(buffer, 0, i - 1);
}

/**
 * @brief Implementation of common, repeated portions of code
 *        - 
 */

#define ARG_DISABLE 0xff

#define MODER_INPUT   (0x0)
#define MODER_OUTPUT  (0x1)
#define MODER_ALT     (0x2)
#define MODER_ANALOG  (0x3)

#define PUPDR_NONE    (0x0)
#define PUPDR_UP      (0x1)
#define PUPDR_DOWN    (0x2)
#define PUPDR_UPDOWN  (0x3)

#define OTYPER_PP     (0x0)
#define OTYPER_OD     (0x1)

static void rcc_ahbenr_GPIOx(GPIO_TypeDef * GPIOx);
static void rcc_ahbenr_GPIOx(GPIO_TypeDef * GPIOx) {
  // Enable the LED GPIOx clock
  #ifdef VVC_F0
    if (GPIOx == GPIOA) {
      RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    }
    else if (GPIOx == GPIOB) {
      RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    }
    else if (GPIOx == GPIOC) {
      RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    }
  #elif  VVC_L0
    if (GPIOx == GPIOA) {
      RCC->IOPENR |= RCC_IOPENR_IOPAEN;
    }
    else if (GPIOx == GPIOB) {
      RCC->IOPENR |= RCC_IOPENR_IOPBEN;
    }
    else if (GPIOx == GPIOC) {
      RCC->IOPENR |= RCC_IOPENR_IOPCEN;
    }
  #endif
}

static void gpiox_config(GPIO_TypeDef * GPIOx, int pin_x, uint8_t moder, uint8_t pupdr, uint8_t otyper, uint8_t af_y);
static void gpiox_config(GPIO_TypeDef * GPIOx, int pin_x, uint8_t moder, uint8_t pupdr, uint8_t otyper, uint8_t af_y) {
  if (af_y != ARG_DISABLE) {
    GPIOx->AFR[pin_x/8] &= ~(0xF << ((pin_x % 8)*4));
    GPIOx->AFR[pin_x/8] |=  (af_y << ((pin_x % 8)*4));
  }
  if (moder != ARG_DISABLE) {
    GPIOx->MODER  &= ~(0x3 << (pin_x*2));
    GPIOx->MODER  |=  (moder << (pin_x*2));
  }
  if (pupdr |= ARG_DISABLE) {
    GPIOx->PUPDR  &= ~(0x3 << (pin_x*2));
    GPIOx->PUPDR  |=  (pupdr << (pin_x*2));
  }
  if (otyper != ARG_DISABLE) {
    GPIOx->OTYPER &= ~(1 << pin_x);
    GPIOx->OTYPER |=  (otyper << pin_x);
  }
}

/**
 * @brief Global variables.
 */
uint32_t core_clock_hz;
volatile uint8_t flags = 0;

/**
 * @brief C representative of LC_I2C class
 * 
 */
LiquidCrystal_I2C LC_I2C = {
  ._I2Cx = I2C_X,
  ._Addr = 0x4E,
  ._displayfunction = 0,  // uninitialized
  ._displaycontrol = 0,   // uninitialized
  ._displaymode = 0,      // uninitialized
  ._numlines = 0,         // uninitialized
  ._oled = 0,
  ._cols = 20,
  ._rows = 4,
  ._backlightval = LCD_NOBACKLIGHT
};

#if ((TEST_NO == 6))
struct buttonPressDetail {
  char *printText;
  int count;
};

struct buttonPressDetail bp_counter[3];

#endif

/**
 * @brief Pointable versions of peripheral/pin defines in main.h
 *        (check test no.)
 * 
 */
//static GPIO_TypeDef * const _i2c_x_GPIOx      = I2C_X_GPIOx;

#if ((TEST_NO == 2) || (TEST_NO == 3) || (TEST_NO == 4) || (TEST_NO == 5))
static GPIO_TypeDef * const _led_GPIOx        = LED_GPIOx;
#endif /* LED_GPIOx */

#if ((TEST_NO == 3))
static GPIO_TypeDef * const _tim_x_ccr_GPIOx  = TIM_X_CCR_GPIOx;
#endif /* TIM_X_CCR_GPIOx */

#if ((TEST_NO == 5) || (TEST_NO == 6))
static GPIO_TypeDef * const _input_pin_GPIOx  = INPUT_GPIOx;
#endif /* TIM_X_CCR_GPIOx */

#if ((TEST_NO == 7))
static TIM_TypeDef * const _tim_x = TIM_X;
#endif /* TIM_X_CCR_GPIOx */

/**
 * ------------------------------------------------
 *                   Test functions
 * ------------------------------------------------
 */

/**
 * @brief Select one of seven tests to run (peripherals used):
 *        - 1: Still text
 *        - 2: Toggle 2 lines       (TIMx)
 *        - 3: Counter              (TIMx)
 *        - 4: Double clock counter (TIMy -> TIMx)
 *        - 5: EXTI button          (EXTI_0)
 *        - 6: 3 buttons            (8-sample TIMx debounce)
 *        - 7: 2 potentiometers     (TIMx -> ADC1 -> DMA_Ch1)
 * 
 */
static void test_device(void);

#if (TEST_NO == 1)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "1: Still text");

  while (1);
}

#elif (TEST_NO == 2)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "2: Toggle 2 lines");

  // Enable the GPIOx clock
  rcc_ahbenr_GPIOx(LED_GPIOx);

  // Setup LED GPIO pin as push-pull output, no pupdr
  gpiox_config(LED_GPIOx, LED_PIN_1, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);

  // Enable the NVIC interrupt for TIMx.
  NVIC_SetPriority(TIM_X_IRQn, 0x03);
  NVIC_EnableIRQ(TIM_X_IRQn);

  // Start the timer (1 second tick).
  reset_timer(TIM_X);
  start_timer(TIM_X, 1000, 1);

  // Print two lines of text (toggle up and down)
  while (1) {
    if (flags & FL_TIMx) {
      /* (1) Clear TIMx bit on flag */
      /* (2) Read LED_PIN, print text based on its value */
      /* (3) Toggle LED_PIN */
      flags &= ~(FL_TIMx);                          /* (1) */

      uint16_t select = _led_GPIOx->IDR;
      if (select & (1<<LED_PIN_1)) {                /* (2) */
        LC_I2C_setCursor(&LC_I2C, 0, 2);
        LC_I2C_print(&LC_I2C, "Hello World!");
        LC_I2C_setCursor(&LC_I2C, 0, 3);
        LC_I2C_print(&LC_I2C, "Cookie         ");
      }
      else {
        LC_I2C_setCursor(&LC_I2C, 0, 2);
        LC_I2C_print(&LC_I2C, "Fryday      ");
        LC_I2C_setCursor(&LC_I2C, 0, 3);
        LC_I2C_print(&LC_I2C, "STM32 Baremetal");
      }
      
      _led_GPIOx->ODR ^= (1 << LED_PIN_1);               /* (3) */
    }
  }
}

#elif (TEST_NO == 3)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "3: Counter");

  // Setup GPIO LED pin
  // Enable the LED GPIOx clock
  rcc_ahbenr_GPIOx(LED_GPIOx);

  // Setup LED pin as push-pull output, no pupdr
  gpiox_config(LED_GPIOx, LED_PIN_1, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);

  // Enable the NVIC interrupt for TIMx.
  NVIC_SetPriority(TIM_X_IRQn, 0x03);
  NVIC_EnableIRQ(TIM_X_IRQn);

  // Reset the timer
  reset_timer(TIM_X);
  
  // Setup appropiate GPIO pins to use as TIMx channel outputs
  // Initialize the GPIOx pins.
  // For TIM3 channels (config...), use PB4,PB5,PA0,PB1 for CH1-4, respectively.

  // Enable the GPIOx peripheral.
  rcc_ahbenr_GPIOx(TIM_X_CCR_GPIOx);

  // Steps to initialize the CCR GPIOx pins:
  // (1) Use alt function AFy (y -> [0, 7])
  // (2) Pins should be set to push-pull low-speed 'alt' mode.
  uint8_t ch_num = ccr_count(TIM_X);

  if (ch_num >= 1) {
    gpiox_config(TIM_X_CCR_GPIOx, TIM_X_CCR_PIN_1, MODER_ALT,
                  PUPDR_NONE, OTYPER_PP, TIM_X_CCR_AFy);
  }
  if (ch_num >= 2) {
    gpiox_config(TIM_X_CCR_GPIOx, TIM_X_CCR_PIN_2, MODER_ALT,
                  PUPDR_NONE, OTYPER_PP, TIM_X_CCR_AFy);
  }
  if (ch_num >= 3) {
    gpiox_config(TIM_X_CCR_GPIOx, TIM_X_CCR_PIN_3, MODER_ALT,
                  PUPDR_NONE, OTYPER_PP, TIM_X_CCR_AFy);
  }
  if (ch_num >= 4) {
    gpiox_config(TIM_X_CCR_GPIOx, TIM_X_CCR_PIN_4, MODER_ALT,
                  PUPDR_NONE, OTYPER_PP, TIM_X_CCR_AFy);
  }

  // Configure CCR channels for TIMx
  ccr_timer(TIM_X, 100, 200, 300, 400);

  // Start the timer (5 second tick).
  start_timer(TIM_X, 500, 1);

  // Increment n every 5 seconds.
  static char str[20];
  static int n = 0;
  while (1) {
    if (flags & FL_TIMx) {
      /* (1) Clear TIMx bit on flag */
      /* (2) Read LED_PIN, print text based on its value */
      /* (3) Toggle LED_PIN */
      flags &= ~(FL_TIMx);                          /* (1) */
      n++;
      itoa((uint32_t)n, str, 10);
      LC_I2C_setCursor(&LC_I2C, 0, 2);
      LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols);
      
      _led_GPIOx->ODR ^= (1 << LED_PIN_1);               /* (3) */
    }
  }
}

#elif (TEST_NO == 4)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "4: 2-clock counter");

  // Enable the LED GPIOx clock
  rcc_ahbenr_GPIOx(LED_GPIOx);
  // Setup LED pin as push-pull output, no pupdr
  gpiox_config(LED_GPIOx, LED_PIN_1, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);

  // Enable the NVIC interrupt for TIMx.
  NVIC_SetPriority(TIM_X_IRQn, 0x03);
  NVIC_EnableIRQ(TIM_X_IRQn);

  // Reset the timer (slave)
  reset_timer(TIM_X);

  // Config the master timer TIM3 -> TIM15 (001)
  // 1: rest the master timer (config)
  reset_timer(TIM_Y);

  // Configure TIMy in master mode
  // For all masters, MMS = 010 (TRGO)
  master_timer(TIM_Y, 0x2);

  // Configure TIMx in slave mode. PSC tick from TRGO (TS = 001)
  // For all slaves, SMS = 111 (external clock mode)
  // Select TS = master clock
  slave_timer(TIM_X, 0x7, TIM_X_TS);

  // 4: start timers in order (5 second tick)
  start_timer_psc_timer(TIM_Y, TIM_X, 1000, 5, 1);

  // Increment n.
  static char str[20];
  static int n = 0;
  while (1) {
    if (flags & FL_TIMx) {
      /* (1) Clear TIMx bit on flag */
      /* (2) Read LED_PIN, print text based on its value */
      /* (3) Toggle LED_PIN */
      flags &= ~(FL_TIMx);                          /* (1) */
      n++;
      itoa((uint32_t)n, str, 10);
      LC_I2C_setCursor(&LC_I2C, 0, 2);
      LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols);
      
      _led_GPIOx->ODR ^= (1 << LED_PIN_1);               /* (3) */
    }
  }
}

#elif (TEST_NO == 5)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "5: EXTI button");

  // Setup GPIO LED pin
  // Enable the LED GPIOx clock
  rcc_ahbenr_GPIOx(LED_GPIOx);

  // Setup LED pin as push-pull output, no pupdr
  gpiox_config(LED_GPIOx, LED_PIN_1, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);
  
  // Enable the EXTI GPIOx clock
  rcc_ahbenr_GPIOx(INPUT_GPIOx);

  // Setup GPIO EXTI pin
  // GPIO inputs. No internal pull-up (external pullups)
  gpiox_config(INPUT_GPIOx, INPUT_1, MODER_INPUT,
                PUPDR_NONE, ARG_DISABLE, ARG_DISABLE);

  // Setup EXTI interrupt pins
  // 1: Enable SYS FG on RCC
  // 2: EXTIx -> PYx (for any x, select Y = A, B, C)
  // 3: Rising edge trigger for EXTIx
  // 4: Enable interrupt generate for EXTIx
  // 5: Enable NVIC interrupt that includes EXTIx
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
  SYSCFG->EXTICR[(INPUT_1/4)] &= ~(0xF << ((INPUT_1 % 4) * 4));

  if (_input_pin_GPIOx == GPIOA) {
    SYSCFG->EXTICR[(INPUT_1/4)] |=  (0x0 << ((INPUT_1 % 4) * 4));
  }
  else if (_input_pin_GPIOx == GPIOB) {
    SYSCFG->EXTICR[(INPUT_1/4)] |=  (0x1 << ((INPUT_1 % 4) * 4));
  }
  else if (_input_pin_GPIOx == GPIOC) {
    SYSCFG->EXTICR[(INPUT_1/4)] |=  (0x2 << ((INPUT_1 % 4) * 4));
  }

  EXTI->RTSR |= (1 << INPUT_1);
  EXTI->IMR |= (1 << INPUT_1);
  NVIC_SetPriority(EXTI_X_IRQn, 0x03);
  NVIC_EnableIRQ(EXTI_X_IRQn);

  // Increment n.
  static char str[20];
  static int n = 0;
  while (1) {
    if (flags & FL_EXTIx) {
      /* (1) Clear EXTIx bit on flag */
      /* (2) Read LED_PIN, print text based on its value */
      /* (3) Toggle LED_PIN */
      flags &= ~(FL_EXTIx);                          /* (1) */
      n++;
      itoa((uint32_t)n, str, 10);
      LC_I2C_setCursor(&LC_I2C, 0, 2);
      LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols);
      
      _led_GPIOx->ODR ^= (1 << LED_PIN_1);               /* (3) */
    }
  }
}

#elif (TEST_NO == 6)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "6: 3 buttons");

  // 3 GPIO LED pins (configure number of pins...)
  // Setup GPIO LED pins
  // Enable the LED GPIOx clock
  rcc_ahbenr_GPIOx(LED_GPIOx);

  // Setup LED pins as push-pull output, no pupdr
  gpiox_config(LED_GPIOx, LED_PIN_1, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);
  gpiox_config(LED_GPIOx, LED_PIN_2, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);
  gpiox_config(LED_GPIOx, LED_PIN_3, MODER_OUTPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);


  // Setup pull-up input
  // Assume all buttons use same GPIOx
  rcc_ahbenr_GPIOx(INPUT_GPIOx);

  // Buttons 1,2,3
  // No internal pull-up, using external pullups...
  gpiox_config(INPUT_GPIOx, INPUT_1, MODER_INPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);
  gpiox_config(INPUT_GPIOx, INPUT_2, MODER_INPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);
  gpiox_config(INPUT_GPIOx, INPUT_3, MODER_INPUT,
                PUPDR_NONE, OTYPER_PP, ARG_DISABLE);

  // Have 3 inputs (config number of inputs/outputs)
  int input_n[3];
  int output_n[3];
  input_n[0]  = INPUT_1, input_n[1]  = INPUT_2; input_n[2]  = INPUT_3;
  output_n[0] = LED_PIN_1, output_n[1] = LED_PIN_2; output_n[2] = LED_PIN_3;
  initInputs(&(input_n[0]), &(output_n[0]), 3);

  // Increment n.
  static char str[20];

  // NEW: Initialize LCD counter display
  for (int i=0; i < 3; i++) {
    bp_counter[i].printText = str + i*6;
    bp_counter[i].count = 0;
    itoa(0, bp_counter[i].printText, 10);
  }

  LC_I2C_setCursor(&LC_I2C, 0, 2);
  LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols);

  // Setup TIMx to check inputs every 1 ms
  // Enable the NVIC interrupt for TIMx.
  NVIC_SetPriority(TIM_X_IRQn, 0x03);
  NVIC_EnableIRQ(TIM_X_IRQn);

  // Start the timer (1 second tick).
  reset_timer(TIM_X);
  start_timer(TIM_X, 1, 10);

  while (1) {
    if (flags & FL_TIMx) {
      /* (1) Clear TIMx bit on flag */
      /* (2) Read input pin, toggle LED when button is pressed/released */
      /* (3) Increment counter by 1 for each button release */
      flags &= ~(FL_TIMx);                          /* (1) */
      
      uint16_t current_state = ~(_input_pin_GPIOx->IDR);
      int results = updateInputs(LED_GPIOx, current_state); /* (2) */
      
      for (int i=0; i < 3; i++) {
        if (results & (INPUT_I << i)) {
          int count = ++bp_counter[i].count;
          itoa((uint32_t) count, bp_counter[i].printText, 10);
          LC_I2C_setCursor(&LC_I2C, 0, 2);
          LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols); /* (3) */
        }
      }
    }
  }
}

#elif (TEST_NO == 7)
static void test_device(void) {
  // Print test label on 2nd line
  LC_I2C_setCursor(&LC_I2C, 0, 1);
  LC_I2C_print(&LC_I2C, "7: 2 Potentiometers");

  // Setup pull-up input
  // Assume all buttons use same GPIOx
  rcc_ahbenr_GPIOx(INPUT_GPIOx);

  // Pots 1, 2
  // Analog inputs
  gpiox_config(INPUT_GPIOx, INPUT_1, MODER_ANALOG,
                ARG_DISABLE, ARG_DISABLE, ARG_DISABLE);
  gpiox_config(INPUT_GPIOx, INPUT_2, MODER_ANALOG,
                ARG_DISABLE, ARG_DISABLE, ARG_DISABLE);

  // Setup ADC
  /* (1) Enable clock to ADC unit. */
  /* (2) Ensure ADC is off. */
  /* (3) Enable ADC channels */
  /* (4) Longest sampling time (F0 239.5 ADC cycles, L0=160.5 ADC cycles) */
  /* (5) Set ADC clock to PCLK/2. Good for external trigger */
  /* (6) 8-bit data resolution. Right align. Single conversion */
  /* (7) External trigger TRGO3 (TIM3 update). Rising edge (config TRGO(TIMx) selection...) */
  /* (8) Calibrate ADC (DMA prerequisite). Wait until ready */
  /* (9) Use with DMA in circular mode */
  /* (10) Enable ADC. Wait until ready */

  RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;                         /* (1) */
  
  ADC1->CR &= ~ADC_CR_ADEN;                                   /* (2) */
  while ((ADC1->ISR & ADC_ISR_ADRDY) == ADC_ISR_ADRDY);

  ADC1->CHSELR |= (1 << ADC1_INz_1) | (1 << ADC1_INz_2);      /* (3) */
  ADC1->SMPR |= ADC_SMPR_SMPR;                               /* (4) */

  ADC1->CFGR2 &= ~(ADC_CFGR2_CKMODE);                         /* (5) */
  ADC1->CFGR2 |=  (ADC_CFGR2_CKMODE_0);

  ADC1->CFGR1 &= ~(ADC_CFGR1_RES | ADC_CFGR1_ALIGN);          /* (6) */
  ADC1->CFGR1 |=  (ADC_CFGR1_RES_1);
  ADC1->CFGR1 &= ~(ADC_CFGR1_CONT);
  ADC1->CFGR1 &= ~(ADC_CFGR1_EXTEN | ADC_CFGR1_EXTSEL);       /* (7) */
  ADC1->CFGR1 |=  (ADC_CFGR1_EXTEN_0);
  ADC1->CFGR1 |=  (ADC1_TRGO << ADC_CFGR1_EXTSEL_Pos);

  ADC1->CR |= ADC_CR_ADCAL;                                   /* (8) */
  while (ADC1->CR & ADC_CR_ADCAL);

  ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;          /* (9) */

  ADC1->CR |= ADC_CR_ADEN;                                    /* (10) */
  while (!(ADC1->ISR & ADC_ISR_ADRDY));

  // Setup DMA (adc_history[100*2] -> 100-sample buffer for each channel)
  // Config channel number? (F0: CH1) (L0: CH1, CH2?)
  static uint16_t adc_history[200];
  /* (1) Enable DMA1 clock. Ensure DMA is disabled */
  /* (2) Direction: ADC peripheral (src) to memory (dst) */
  /* (3) Circular mode enabled */
  /* (4) src and dat contain 16-bit data */
  /* (5) Memory address increment only */
  /* (6) DMA channel has High priority */
  /* (7) 100*2 data to move */
  /* (8) peri_addr = ADC data. mem_addr = setup parameter */
  /* (9) Enable DMA1_Channel1 transfer complete interrupt */
  /* (10) Enable DMA Channnel1 */

  RCC->AHBENR |= RCC_AHBENR_DMA1EN; /* (1) */
  DMA1_Channel1->CCR &= ~DMA_CCR_EN;

  DMA1_Channel1->CCR &= ~DMA_CCR_MEM2MEM; /* (2) */
  DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
  DMA1_Channel1->CCR |= DMA_CCR_CIRC; /* (3) */
  DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0;  /* (4) */
  DMA1_Channel1->CCR |= DMA_CCR_MINC; /* (5) */
  DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
  DMA1_Channel1->CCR |= DMA_CCR_PL_1; /* (6) */

  DMA1_Channel1->CNDTR = 100*2; /* (7) */
  DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));  /* (8) */
  DMA1_Channel1->CMAR = (uint32_t)(&(adc_history[0]));

  DMA1_Channel1->CCR |= DMA_CCR_TCIE; /* (9) */
  NVIC_SetPriority(DMA1_Channel1_IRQn, 0x03);
  NVIC_EnableIRQ(DMA1_Channel1_IRQn);

  DMA1_Channel1->CCR |= DMA_CCR_EN;

  // Setup timer 3 (config...)
  // Reset the timer (1 ms tick).
  reset_timer(TIM_X);

  // Set update event as TRGO event (MMS = 010)
  master_timer(TIM_X, 0x2);

  // Custom start_timer(). No timer interrupts
  _tim_x->PSC   = ((core_clock_hz / 1000) / 10) - 1;
  _tim_x->ARR   = (1 * 10) - 1;
  // Send an update event to reset the timer and apply settings.
  _tim_x->EGR  |= TIM_EGR_UG;
  // Enable the timer.
  _tim_x->CR1  |= TIM_CR1_CEN;

  // Start ADC
  ADC1->CR |= ADC_CR_ADSTART;

  // Setup LCD text
  static char str[20];
  itoa(0, str, 10);
  itoa(0, str+10, 10);
  LC_I2C_setCursor(&LC_I2C, 0, 2);
  LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols);

  while (1) {
    if (flags & FL_DMAx) {
      /* (1) Clear DMAx bit on flag */
      /* (2) Read input pin, toggle LED when button is pressed/released */
      /* (3) Increment counter by 1 for each button release */
      flags &= ~(FL_DMAx);                          /* (1) */
      
      // New
      uint32_t adc_avg1 = 0;
      uint32_t adc_avg2 = 0;
      for (int i=0; i<100; i++) {
        adc_avg1 += (uint32_t)adc_history[i*2];
        adc_avg2 += (uint32_t)adc_history[i*2 + 1];
      }
      adc_avg1 = adc_avg1 / 100;
      adc_avg2 = adc_avg2 / 100;

      itoa(adc_avg1, str, 10);
      itoa(adc_avg2, str+10, 10);
      LC_I2C_setCursor(&LC_I2C, 0, 2);
      LC_I2C_print_pad(&LC_I2C, str, LC_I2C._cols);
    }
  }
}
#endif

/**
 * ------------------------------------------------
 *                   Main program.
 * ------------------------------------------------
 */
int main(void) {
  // Initial clock setup.
  #ifdef VVC_F0
    // Reset the Flash 'Access Control Register', and
    // then set 1 wait-state and enable the prefetch buffer.
    // (The device header files only show 1 bit for the F0
    //  line, but the reference manual shows 3...)
    FLASH->ACR &= ~(0x00000017);
    FLASH->ACR |=  (FLASH_ACR_LATENCY |
                    FLASH_ACR_PRFTBE);
    // Configure the PLL to (HSI / 2) * 12 = 48MHz.
    // Use a PLLMUL of 0xA for *12, and keep PLLSRC at 0
    // to use (HSI / PREDIV) as the core source. HSI = 8MHz.
    RCC->CFGR  &= ~(RCC_CFGR_PLLMUL |
                    RCC_CFGR_PLLSRC);
    RCC->CFGR  |=  (RCC_CFGR_PLLSRC_HSI_DIV2 |
                    RCC_CFGR_PLLMUL12);
    // Turn the PLL on and wait for it to be ready.
    RCC->CR    |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
    // The system clock is now 48MHz.
    core_clock_hz = 48000000;
  #elif VVC_L0
    // Set the Flash ACR to use 1 wait-state
    // and enable the prefetch buffer and pre-read.
    FLASH->ACR |=  (FLASH_ACR_LATENCY |
                    FLASH_ACR_PRFTEN |
                    FLASH_ACR_PRE_READ);
    // Enable the HSI oscillator, since the L0 series boots
    // to the MSI one.
    RCC->CR    |=  (RCC_CR_HSION);
    while (!(RCC->CR & RCC_CR_HSIRDY)) {};
    // Configure the PLL to use HSI16 with a PLLDIV of
    // 2 and PLLMUL of 4.
    RCC->CFGR  &= ~(RCC_CFGR_PLLDIV |
                    RCC_CFGR_PLLMUL |
                    RCC_CFGR_PLLSRC);
    RCC->CFGR  |=  (RCC_CFGR_PLLDIV2 |
                    RCC_CFGR_PLLMUL4 |
                    RCC_CFGR_PLLSRC_HSI);
    // Enable the PLL and wait for it to stabilize.
    RCC->CR    |=  (RCC_CR_PLLON);
    while (!(RCC->CR & RCC_CR_PLLRDY)) {};
    // Select the PLL as the system clock source.
    RCC->CFGR  &= ~(RCC_CFGR_SW);
    RCC->CFGR  |=  (RCC_CFGR_SW_PLL);
    while (!(RCC->CFGR & RCC_CFGR_SWS_PLL)) {};
    // Set the global clock speed variable.
    core_clock_hz = 32000000;
  #endif

  // Enable the I2C peripheral in 'RCC_APB1ENR'.
  if      (I2C_X == I2C1) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  }
  #ifdef VVC_F0
  else if (I2C_X == I2C2) {
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
  }
  #endif /* I2C2 */

  rcc_ahbenr_GPIOx(I2C_X_GPIOx);

  // Initialize the I2Cx GPIOx pins.
  //    All I2C pins use AF1 (except L0: I2C(SCL,SDA) = (PB8,PB9) -> AF4). (configurable, but don't)
  //    Pins should be set to 'alt' mode, open-drain, with pull-up.
  gpiox_config(I2C_X_GPIOx, PX_SCL, MODER_ALT,
                PUPDR_UP, OTYPER_OD, 0x1);
  gpiox_config(I2C_X_GPIOx, PX_SDA, MODER_ALT,
                PUPDR_UP, OTYPER_OD, 0x1);

  // Initialize delay.c
  /* (1) millis(), milli_wait() */
  /* (2) micros(), micro_wait() */
  systick_init((uint32_t) core_clock_hz);   /* (1) */
  timer_us_init((uint32_t) core_clock_hz);  /* (2) */

  // Initialize LC_I2C display
  /* (1) Print header text */
  LC_I2C_init(&LC_I2C);
  LC_I2C_backlight(&LC_I2C);
  LC_I2C_setCursor(&LC_I2C, 0, 0);
  LC_I2C_print(&LC_I2C, "Running test no...");  /* (1) */

  // Run desired test no.
  test_device();
}
