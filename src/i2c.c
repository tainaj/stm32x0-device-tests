#include "i2c.h"

uint8_t dec_to_bcd(uint8_t dec)
{ // originally inline
  return ((((dec / 10) & 0xF) << 4) |
          ((dec % 10) & 0xF));
}

void i2c_init(I2C_TypeDef * I2Cx, uint8_t addr)
{
  // Initialize the I2C1 peripheral.
  // First, disable the peripheral.
  I2Cx->CR1 &= ~(I2C_CR1_PE);
  // Clear some 'CR1' bits.
  I2Cx->CR1 &= ~(I2C_CR1_DNF |
                 I2C_CR1_ANFOFF |
                 I2C_CR1_SMBHEN |
                 I2C_CR1_SMBDEN);
  // Clear some 'CR2' bits.
  I2Cx->CR2 &= ~(I2C_CR2_RD_WRN |
                 I2C_CR2_NACK |
                 I2C_CR2_RELOAD |
                 I2C_CR2_AUTOEND);
  // Clear all 'ICR' flags.
  I2Cx->ICR |= (I2C_ICR_ADDRCF |
                I2C_ICR_NACKCF |
                I2C_ICR_STOPCF |
                I2C_ICR_BERRCF |
                I2C_ICR_ARLOCF |
                I2C_ICR_OVRCF |
                I2C_ICR_PECCF |
                I2C_ICR_TIMOUTCF |
                I2C_ICR_ALERTCF);
  // Configure I2C timing.
  // Reset all but the reserved bits.
  I2Cx->TIMINGR &= (0x0F000000);
  // (100KHz @48MHz core clock, according to an application note)
  I2Cx->TIMINGR |= (0xB0420F13);
  // Enable the peripheral. (PE = 'Peripheral Enable')
  I2Cx->CR1 |= I2C_CR1_PE;

  // Set the LCD text display's I2C address.
  I2Cx->CR2 &= ~(I2C_CR2_SADD);
  I2Cx->CR2 |= (addr << I2C_CR2_SADD_Pos);
  // Write one byte; (D7 D6 D5 D4 BL E  RW Rs)
  I2Cx->CR2 &= ~(I2C_CR2_NBYTES);
  I2Cx->CR2 |=  (0x01 << I2C_CR2_NBYTES_Pos);
}

void i2c_start(I2C_TypeDef * I2Cx)
{ // originally inline
  // Send 'Start' condition, and wait for acknowledge.
  I2Cx->CR2 |= (I2C_CR2_START);
  while ((I2Cx->CR2 & I2C_CR2_START))
  {
  }
}

void i2c_stop(I2C_TypeDef * I2Cx)
{ // originally inline
  // Send 'Stop' condition, and wait for acknowledge.
  I2Cx->CR2 |= (I2C_CR2_STOP);
  while ((I2Cx->CR2 & I2C_CR2_STOP))
  {
  }
  // Reset the ICR ('Interrupt Clear Register') event flag.
  I2Cx->ICR |= (I2C_ICR_STOPCF);
  while ((I2Cx->ICR & I2C_ICR_STOPCF))
  {
  }
}

void i2c_write_byte(I2C_TypeDef * I2Cx, uint8_t dat)
{
  I2Cx->TXDR = (I2Cx->TXDR & 0xFFFFFF00) | dat;
  // Wait for one of these ISR bits:
  // 'TXIS' ("ready for next byte")
  // 'TC'   ("transfer complete")
  while (!(I2Cx->ISR & (I2C_ISR_TXIS | I2C_ISR_TC)))
  {
  }
  // (Also of interest: 'TXE' ("TXDR register is empty") and
  //  'TCR' ("transfer complete, and 'RELOAD' is set."))
}

/*uint8_t i2c_read_byte(void)
{
  // Wait for a byte of data to be available, then read it.
  while (!(I2C1->ISR & I2C_ISR_RXNE))
  {
  }
  return (I2C1->RXDR & 0xFF);
}

uint8_t i2c_read_register(uint8_t reg_addr)
{
  // Set '1 byte to send.'
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= (0x01 << I2C_CR2_NBYTES_Pos);
  // Start the I2C write transmission.
  i2c_start();
  // Send the register address.
  i2c_write_byte(reg_addr);
  // Stop the I2C write transmission.
  i2c_stop();
  // Set '1 byte to receive.'
  I2C1->CR2 &= ~(I2C_CR2_NBYTES);
  I2C1->CR2 |= (0x01 << I2C_CR2_NBYTES_Pos);
  // Set 'read' I2C direction.
  I2C1->CR2 |= (I2C_CR2_RD_WRN);
  // Start the I2C read transmission.
  i2c_start();
  // Read the transmitted data.
  uint8_t read_result = i2c_read_byte();
  // Stop the I2C read transmission.
  i2c_stop();
  // Set 'write' I2C direction again.
  I2C1->CR2 &= ~(I2C_CR2_RD_WRN);

  // Return the read value.
  return read_result;
}*/
