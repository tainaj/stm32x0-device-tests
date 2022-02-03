//YWROBOT
#ifndef LiquidCrystal_I2C_h
#define LiquidCrystal_I2C_h

//#include <stdint.h> //#include <inttypes.h>
//#include <stdio.h> //#include "Print.h"
#include "i2c.h" //#include <Wire.h>
#include "global.h"

// commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME 0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT 0x10
#define LCD_FUNCTIONSET 0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags for display entry mode
#define LCD_ENTRYRIGHT 0x00
#define LCD_ENTRYLEFT 0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

// flags for display/cursor shift
#define LCD_DISPLAYMOVE 0x08
#define LCD_CURSORMOVE 0x00
#define LCD_MOVERIGHT 0x04
#define LCD_MOVELEFT 0x00

// flags for function set
#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE 0x08
#define LCD_1LINE 0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS 0x00

// flags for backlight control
#define LCD_BACKLIGHT 0x08
#define LCD_NOBACKLIGHT 0x00

#define En 0x04  // Enable bit
#define Rw 0x02  // Read/Write bit
#define Rs 0x01  // Register select bit

/**
 * \brief Provides command-level api to interact with the LCD-I2C display module.
 */

typedef struct LiquidCrystal_I2C {
  // Private variables
  uint8_t _Addr;
  uint8_t _displayfunction;
  uint8_t _displaycontrol;
  uint8_t _displaymode;
  uint8_t _numlines;
  uint8_t _oled; // = false
  uint8_t _cols;
  uint8_t _rows;
  uint8_t _backlightval;
} LiquidCrystal_I2C;

/**
 * \brief Public functions
 */

void LC_I2C_begin(LiquidCrystal_I2C *this, uint8_t cols, uint8_t rows, uint8_t lcd_rows);
void LC_I2C_clear(LiquidCrystal_I2C *this);
void LC_I2C_home(LiquidCrystal_I2C *this);
//void LC_I2C_noDisplay(LiquidCrystal_I2C *this);
void LC_I2C_display(LiquidCrystal_I2C *this);
/*void LC_I2C_noBlink(LiquidCrystal_I2C *this);
void LC_I2C_blink(LiquidCrystal_I2C *this);
void LC_I2C_noCursor(LiquidCrystal_I2C *this);
void LC_I2C_cursor(LiquidCrystal_I2C *this);
void LC_I2C_scrollDisplayLeft(LiquidCrystal_I2C *this);
void LC_I2C_scrollDisplayRight(LiquidCrystal_I2C *this);
void LC_I2C_printLeft(LiquidCrystal_I2C *this);
void LC_I2C_printRight(LiquidCrystal_I2C *this);
void LC_I2C_leftToRight(LiquidCrystal_I2C *this);
void LC_I2C_rightToLeft(LiquidCrystal_I2C *this);
void LC_I2C_shiftIncrement(LiquidCrystal_I2C *this);
void LC_I2C_shiftDecrement(LiquidCrystal_I2C *this);
void LC_I2C_noBacklight(LiquidCrystal_I2C *this);*/
void LC_I2C_backlight(LiquidCrystal_I2C *this);
/*void LC_I2C_autoscroll(LiquidCrystal_I2C *this);
void LC_I2C_noAutoscroll(LiquidCrystal_I2C *this);*/
void LC_I2C_createChar(LiquidCrystal_I2C *this, uint8_t location, uint8_t charmap[]);
// Example: 	const char bell[8] PROGMEM = {B00100,B01110,B01110,B01110,B11111,B00000,B00100,B00000};

void LC_I2C_setCursor(LiquidCrystal_I2C *this, uint8_t, uint8_t); 
void LC_I2C_print(LiquidCrystal_I2C *this, const char *str);
void LC_I2C_printByte(LiquidCrystal_I2C *this, uint8_t byte);
  
//void LC_I2C_command(LiquidCrystal_I2C *this, uint8_t);
void LC_I2C_init(LiquidCrystal_I2C *this);
//void LC_I2C_oled_init(LiquidCrystal_I2C *this);

////compatibility API function aliases
/*void LC_I2C_blink_on(LiquidCrystal_I2C *this);						// alias for blink()
void LC_I2C_blink_off(LiquidCrystal_I2C *this);       					// alias for noBlink()
void LC_I2C_cursor_on(LiquidCrystal_I2C *this);      	 					// alias for cursor()
void LC_I2C_cursor_off(LiquidCrystal_I2C *this);      					// alias for noCursor()
void LC_I2C_setBacklight(LiquidCrystal_I2C *this, uint8_t new_val);				// alias for backlight() and nobacklight()
void LC_I2C_load_custom_character(LiquidCrystal_I2C *this, uint8_t char_num, uint8_t *rows);	// alias for createChar()
void LC_I2C_printstr(LiquidCrystal_I2C *this, const char[]);*/

////Unsupported API functions (not implemented in this library)
/*uint8_t LC_I2C_status(LiquidCrystal_I2C *this);
void LC_I2C_setContrast(LiquidCrystal_I2C *this, uint8_t new_val);
uint8_t LC_I2C_keypad(LiquidCrystal_I2C *this);
void LC_I2C_setDelay(LiquidCrystal_I2C *this, int,int);
void LC_I2C_on(LiquidCrystal_I2C *this);
void LC_I2C_off(LiquidCrystal_I2C *this);
uint8_t LC_I2C_init_bargraph(LiquidCrystal_I2C *this, uint8_t graphtype);
void LC_I2C_draw_horizontal_graph(LiquidCrystal_I2C *this, uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);
void LC_I2C_draw_vertical_graph(LiquidCrystal_I2C *this, uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end);*/

/**
 * \brief Private functions (implement as static functions in .c file, then remove from .h file)
 */

//void init_priv(LiquidCrystal_I2C *this);
/*void send(LiquidCrystal_I2C *this, uint8_t, uint8_t);
void write4bits(LiquidCrystal_I2C *this, uint8_t);
void expanderWrite(LiquidCrystal_I2C *this, uint8_t);
void pulseEnable(LiquidCrystal_I2C *this, uint8_t);*/

#endif
