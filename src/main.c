#include "main.h"

#define TEST 2

/**
 * @brief Global variables.
 */
uint32_t core_clock_hz;

/**
 * @brief C representative of LC_I2C class
 * 
 */
LiquidCrystal_I2C LC_I2C = {
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

#if TEST == 2 // CustomChar: def for all 8 custom characters
  uint8_t bell[8]     = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
  uint8_t note[8]     = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
  uint8_t clock[8]    = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
  uint8_t heart[8]    = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
  uint8_t duck[8]     = {0x0,0xc,0x1d,0xf,0xf,0x6,0x0};
  uint8_t check[8]    = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
  uint8_t cross[8]    = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
  uint8_t retarrow[8] = {0x1,0x1,0x5,0x9,0x1f,0x8,0x4};
#endif

/**
 * Main program.
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

  // Enable the I2C1 peripheral in 'RCC_APB1ENR'.
  RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
  #ifdef VVC_F0
    // Enable the GPIOB peripheral in 'RCC_AHBENR'.
    RCC->AHBENR   |= RCC_AHBENR_GPIOBEN;
  #elif  VVC_L0
    RCC->IOPENR   |= RCC_IOPENR_IOPBEN;
  #endif
  // Initialize the GPIOB pins.
  // For I2C1, use AF1.
  GPIOB->AFR[PB_SCL/8] &= ~(0xF << (PB_SCL*4));
  GPIOB->AFR[PB_SCL/8] |=  (0x1 << (PB_SCL*4));
  GPIOB->AFR[PB_SDA/8] &= ~(0xF << (PB_SDA*4));
  GPIOB->AFR[PB_SDA/8] |=  (0x1 << (PB_SDA*4));
  // B6/7 should be set to 'alt' mode, open-drain, with pull-up.
  GPIOB->MODER  &= ~(0x3 << (PB_SCL*2));
  GPIOB->MODER  |=  (0x2 << (PB_SCL*2));
  GPIOB->PUPDR  &= ~(0x3 << (PB_SCL*2));
  GPIOB->PUPDR  |=  (0x1 << (PB_SCL*2));
  GPIOB->OTYPER |=  (0x1 << PB_SCL);
  GPIOB->MODER  &= ~(0x3 << (PB_SDA*2));
  GPIOB->MODER  |=  (0x2 << (PB_SDA*2));
  GPIOB->PUPDR  &= ~(0x3 << (PB_SDA*2));
  GPIOB->PUPDR  |=  (0x1 << (PB_SDA*2));
  GPIOB->OTYPER |=  (0x1 << PB_SDA);
  
  #if TEST == 1 // HelloWorld
    LC_I2C_init(&LC_I2C); // initialize the lcd
    // Print a message to the LCD.
    LC_I2C_backlight(&LC_I2C);
    LC_I2C_setCursor(&LC_I2C, 3,0);
    LC_I2C_print(&LC_I2C, "Hello, world!");
    LC_I2C_setCursor(&LC_I2C, 2,1);
    LC_I2C_print(&LC_I2C, "Ywrobot STM32!");
    LC_I2C_setCursor(&LC_I2C, 0,2);
    LC_I2C_print(&LC_I2C, "STM32 LCM IIC 2022");
    LC_I2C_setCursor(&LC_I2C, 2,3);
    LC_I2C_print(&LC_I2C, "Power By jtaina!");
  #elif TEST == 2 // Customchar
    LC_I2C_init(&LC_I2C);       // initialize the lcd
    LC_I2C_backlight(&LC_I2C);

    LC_I2C_createChar(&LC_I2C, 0, bell);
    LC_I2C_createChar(&LC_I2C, 1, note);
    LC_I2C_createChar(&LC_I2C, 2, clock);
    LC_I2C_createChar(&LC_I2C, 3, heart);
    LC_I2C_createChar(&LC_I2C, 4, duck);
    LC_I2C_createChar(&LC_I2C, 5, check);
    LC_I2C_createChar(&LC_I2C, 6, cross);
    LC_I2C_createChar(&LC_I2C, 7, retarrow);
    LC_I2C_home(&LC_I2C);

    LC_I2C_print(&LC_I2C, "Hello world...");
    LC_I2C_setCursor(&LC_I2C, 0, 1);
    LC_I2C_print(&LC_I2C, " i ");
    LC_I2C_printByte(&LC_I2C, 3);
    LC_I2C_print(&LC_I2C, "stm32s!");
    delay(5000 * 1000);

    uint8_t i = 0;
    while (1) {
      LC_I2C_clear(&LC_I2C);
      LC_I2C_print(&LC_I2C, "Codes 0x");
      LC_I2C_print(&LC_I2C, "-0x");
      LC_I2C_setCursor(&LC_I2C, 0, 1);
      for (int j=0; j<16; j++) {
        LC_I2C_printByte(&LC_I2C, i+j);
      }
      i+=16;

      delay(4000 * 1000);
    }
  #endif
    
  while (1);
}
