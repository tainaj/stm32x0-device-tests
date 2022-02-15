#ifndef _VVC_UTIL_H
#define _VVC_UTIL_H

#include "global.h"

uint8_t dec_to_bcd(uint8_t dec); // originally inline
void i2c_init (I2C_TypeDef * I2Cx, uint8_t addr);
void i2c_start(I2C_TypeDef * I2Cx); // originally inline
void i2c_stop (I2C_TypeDef * I2Cx); // originally inline
void i2c_write_byte(I2C_TypeDef * I2Cx, uint8_t dat);
//uint8_t i2c_read_byte(void);
//uint8_t i2c_read_register(uint8_t reg_addr);

#endif
