#ifndef _VVC_UTIL_H
#define _VVC_UTIL_H

#include "global.h"

// Define GPIOB pin mappings for I2C
#define PB_SCL (6)
#define PB_SDA (7)

uint8_t dec_to_bcd(uint8_t dec); // originally inline
void i2c_init(uint8_t addr);
void i2c_start(void); // originally inline
void i2c_stop(void); // originally inline
void    i2c_write_byte(uint8_t dat);
uint8_t i2c_read_byte(void);
uint8_t i2c_read_register(uint8_t reg_addr);

#endif
