.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

// Global memory locations
.global vtable
.global default_interrupt_handler

/*
 * The vector table. STM files for my STM32F0DISCOVERY board contains
 * additional functionality not utilized by the board used by
 * 'Bare Bones'. Margin comments will highlight non-trivial difference
 * between the boards.
 *
 * Update: These additional functionalities will be added back in.
 */
.type vtable, %object
.section .vector_table,"a",%progbits
vtable:
  // 0-15
  .word _estack
  .word reset_handler
  .word NMI_handler
  .word hard_fault_handler
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word 0
  .word SVC_handler
  .word 0
  .word 0
  .word pending_SV_handler
  .word SysTick_handler
  // 16-31
  .word window_watchdog_IRQ_handler
  .word PVD_IRQ_handler
  .word RTC_IRQ_handler
  .word flash_IRQ_handler
  .word RCC_CRS_IRQ_handler             // RCC_CRS_IRQHandler
  .word EXTI0_1_IRQ_handler
  .word EXTI2_3_IRQ_handler
  .word EXTI4_15_IRQ_handler
  .word TSC_IRQ_handler                 // TSC_IRQHandler
  .word DMA1_chan1_IRQ_handler
  .word DMA1_chan2_3_IRQ_handler
  .word DMA1_chan4_5_IRQ_handler
  .word ADC1_COMP_IRQ_handler            // ADC1_COMP_IRQHandler
  .word TIM1_BRK_UP_TRG_COM_IRQ_handler      // TIM1_BRK_UP_TRG_COM_IRQHandler
  .word TIM1_CC_IRQ_handler
  .word TIM2_IRQ_handler
  // 32-47
  .word TIM3_IRQ_handler
  .word TIM6_DAC_IRQ_handler        // TIM6_DAC_IRQHandler
  .word 0
  .word TIM14_IRQ_handler
  .word TIM15_IRQ_handler           // TIM15_IRQHandler
  .word TIM16_IRQ_handler
  .word TIM17_IRQ_handler
  .word I2C1_IRQ_handler
  .word I2C2_IRQ_handler            // I2C2_IRQHandler
  .word SPI1_IRQ_handler
  .word SPI2_IRQ_handler            // SPI2_IRQHandler
  .word USART1_IRQ_handler
  .word USART2_IRQ_handler          // USART2_IRQHandler
  .word 0
  .word CEC_CAN_IRQ_handler         // CEC_CAN_IRQHandler
  .word 0
  // 48
  // (Location to boot from for RAM startup)
  #define boot_ram_base  0xF108F85F
  .word boot_ram_base

  /*
   * Setup weak aliases for each exception handler to the
   * default one. These can be updated later, or just
   * overridden since they're weak refs.
   * The reset_handler is set up seperately.
   */
  .weak NMI_handler
  .thumb_set NMI_handler,default_interrupt_handler
  .weak hard_fault_handler
  .thumb_set hard_fault_handler,default_interrupt_handler
  .weak SVC_handler
  .thumb_set SVC_handler,default_interrupt_handler
  .weak pending_SV_handler
  .thumb_set pending_SV_handler,default_interrupt_handler
  .weak SysTick_handler
  .thumb_set SysTick_handler,default_interrupt_handler
  .weak window_watchdog_IRQ_handler
  .thumb_set window_watchdog_IRQ_handler,default_interrupt_handler
  .weak PVD_IRQ_handler
  .thumb_set PVD_IRQ_handler,default_interrupt_handler
  .weak RTC_IRQ_handler
  .thumb_set RTC_IRQ_handler,default_interrupt_handler
  .weak flash_IRQ_handler
  .thumb_set flash_IRQ_handler,default_interrupt_handler
  .weak RCC_CRS_IRQ_handler
  .thumb_set RCC_CRS_IRQ_handler,default_interrupt_handler
  .weak EXTI0_1_IRQ_handler
  .thumb_set EXTI0_1_IRQ_handler,default_interrupt_handler
  .weak EXTI2_3_IRQ_handler
  .thumb_set EXTI2_3_IRQ_handler,default_interrupt_handler
  .weak EXTI4_15_IRQ_handler
  .thumb_set EXTI4_15_IRQ_handler,default_interrupt_handler
  .weak TSC_IRQ_handler
  .thumb_set TSC_IRQ_handler,default_interrupt_handler
  .weak DMA1_chan1_IRQ_handler
  .thumb_set DMA1_chan1_IRQ_handler,default_interrupt_handler
  .weak DMA1_chan2_3_IRQ_handler
  .thumb_set DMA1_chan2_3_IRQ_handler,default_interrupt_handler
  .weak DMA1_chan4_5_IRQ_handler
  .thumb_set DMA1_chan4_5_IRQ_handler,default_interrupt_handler
  .weak ADC1_COMP_IRQ_handler
  .thumb_set ADC1_COMP_IRQ_handler,default_interrupt_handler
  .weak TIM1_BRK_UP_TRG_COM_IRQ_handler
  .thumb_set TIM1_BRK_UP_TRG_COM_IRQ_handler,default_interrupt_handler
  .weak TIM1_CC_IRQ_handler
  .thumb_set TIM1_CC_IRQ_handler,default_interrupt_handler
  .weak TIM2_IRQ_handler
  .thumb_set TIM2_IRQ_handler,default_interrupt_handler
  .weak TIM3_IRQ_handler
  .thumb_set TIM3_IRQ_handler,default_interrupt_handler
  .weak TIM6_DAC_IRQ_handler
  .thumb_set TIM6_DAC_IRQ_handler,default_interrupt_handler
  .weak TIM14_IRQ_handler
  .thumb_set TIM14_IRQ_handler,default_interrupt_handler
  .weak TIM15_IRQ_handler
  .thumb_set TIM15_IRQ_handler,default_interrupt_handler
  .weak TIM16_IRQ_handler
  .thumb_set TIM16_IRQ_handler,default_interrupt_handler
  .weak TIM17_IRQ_handler
  .thumb_set TIM17_IRQ_handler,default_interrupt_handler
  .weak I2C1_IRQ_handler
  .thumb_set I2C1_IRQ_handler,default_interrupt_handler
  .weak I2C2_IRQ_handler
  .thumb_set I2C2_IRQ_handler,default_interrupt_handler
  .weak SPI1_IRQ_handler
  .thumb_set SPI1_IRQ_handler,default_interrupt_handler
  .weak SPI2_IRQ_handler
  .thumb_set SPI2_IRQ_handler,default_interrupt_handler
  .weak USART1_IRQ_handler
  .thumb_set USART1_IRQ_handler,default_interrupt_handler
  .weak USART2_IRQ_handler
  .thumb_set USART2_IRQ_handler,default_interrupt_handler
  .weak CEC_CAN_IRQ_handler
  .thumb_set CEC_CAN_IRQ_handler,default_interrupt_handler
.size vtable, .-vtable

/*
 * A 'Default' interrupt handler. This is where interrupts
 * which are not otherwise configured will go.
 * It is an infinite loop, because...well, we weren't
 * expecting the interrupt, so what can we do?
 */
.section .text.default_interrupt_handler,"ax",%progbits
default_interrupt_handler:
  default_interrupt_loop:
    B default_interrupt_loop
.size default_interrupt_handler, .-default_interrupt_handler

