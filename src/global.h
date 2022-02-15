#ifndef _VVC_GLOBAL_H
#define _VVC_GLOBAL_H

#include <stdio.h>
#include <stdint.h>
#ifdef VVC_F0
  #include "stm32f0xx.h"
#elif  VVC_L0
  #include "stm32l0xx.h"
#endif

// Global variables.
extern uint32_t core_clock_hz;
extern volatile uint8_t flags;

// Flag bits (set by ISR/event, cleared by test_device() loop)
#define FL_TIMx   0x01
#define FL_EXTIx  0x02
#define FL_DMAx   0x04

#endif
