#ifndef _VVC_GLOBAL_H
#define _VVC_GLOBAL_H

#include <stdint.h>
#ifdef VVC_F0
  #include "stm32f0xx.h"
#elif  VVC_L0
  #include "stm32l0xx.h"
#endif

// Micro wait function (MCU_CLASS dependent asm file)
void delay(int);

// Global variables.
extern uint32_t core_clock_hz;

#endif
