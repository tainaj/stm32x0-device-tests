// Based on the work by DFRobot

#include "LiquidCrystal_I2C.h"

#define printIIC(args)	i2c_write_byte(args)

/**
 * @brief Special low-level function: send byte to LC_I2C
 * 
 * @param this 	class object
 * @param value byte to be written 
 */
static inline void LC_I2C_write(LiquidCrystal_I2C *this, uint8_t value);

/**
 * @brief Low-level and mid-level private functions
 * 
 */

static inline void command(LiquidCrystal_I2C *this, uint8_t);

static void init_priv(LiquidCrystal_I2C *this);
static void send(LiquidCrystal_I2C *this, uint8_t, uint8_t);
static void write4bits(LiquidCrystal_I2C *this, uint8_t);
static void expanderWrite(LiquidCrystal_I2C *this, uint8_t);
static void pulseEnable(LiquidCrystal_I2C *this, uint8_t);


// When the display powers up, it is configured as follows:
//
// 1. Display clear
// 2. Function set: 
//    DL = 1; 8-bit interface data 
//    N = 0; 1-line display 
//    F = 0; 5x8 dot character font 
// 3. Display on/off control: 
//    D = 0; Display off 
//    C = 0; Cursor off 
//    B = 0; Blinking off 
// 4. Entry mode set: 
//    I/D = 1; Increment by 1
//    S = 0; No shift 
//
// Note, however, that resetting the Arduino doesn't reset the LCD, so we
// can't assume that its in that state when a sketch starts (and the
// LiquidCrystal constructor is called).

/**
 * \brief Private functions
 */

static inline void LC_I2C_write(LiquidCrystal_I2C *this, uint8_t value) {
	send(this, value, Rs);
}

static void init_priv(LiquidCrystal_I2C *this)
{
	i2c_init(this->_Addr);
	this->_displayfunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;
	LC_I2C_begin(this, this->_cols, this->_rows, LCD_5x8DOTS);  
}

/************ low level data pushing commands **********/

// write either command or data
static void send(LiquidCrystal_I2C *this, uint8_t value, uint8_t mode) {
	uint8_t highnib=value&0xf0;
	uint8_t lownib=(value<<4)&0xf0;
    write4bits(this, (highnib)|mode);
	write4bits(this, (lownib)|mode); 
}

static void write4bits(LiquidCrystal_I2C *this, uint8_t value) {
	expanderWrite(this, value);
	pulseEnable(this, value);
}

static void expanderWrite(LiquidCrystal_I2C *this, uint8_t _data){                                        
	i2c_start();
	printIIC((int)(_data) | this->_backlightval);
	i2c_stop();
}

static void pulseEnable(LiquidCrystal_I2C *this, uint8_t _data){
	expanderWrite(this, _data | En);	// En high
	delay(1);		// enable pulse must be >450ns
	
	expanderWrite(this, _data & ~En);	// En low
	delay(50);		// commands need > 37us to settle
}

/**
 * \brief Public API
 */

/*void LC_I2C_oled_init(){
  _oled = true;
	init_priv();
}*/

void LC_I2C_init(LiquidCrystal_I2C *this){
	init_priv(this);
}

void LC_I2C_begin(LiquidCrystal_I2C *this, uint8_t cols, uint8_t lines, uint8_t dotsize) {
	if (lines > 1) {
		this->_displayfunction |= LCD_2LINE;
	}
	this->_numlines = lines;

	// for some 1 line displays you can select a 10 pixel high font
	if ((dotsize != 0) && (lines == 1)) {
		this->_displayfunction |= LCD_5x10DOTS;
	}
    
	// SEE PAGE 45/46 FOR INITIALIZATION SPECIFICATION!
	// according to datasheet, we need at least 40ms after power rises above 2.7V
	// before sending commands. Arduino can turn on way befer 4.5V so we'll wait 50
	delay(50 * 1000); 
  
	// Now we pull both RS and R/W low to begin commands
	expanderWrite(this, this->_backlightval);	// reset expanderand turn backlight off (Bit 8 =1)
	delay(1000 * 1000);

  	//put the LCD into 4 bit mode
	// this is according to the hitachi HD44780 datasheet
	// figure 24, pg 46
	
	// we start in 8bit mode, try to set 4 bit mode
    write4bits(this, 0x03 << 4);
    delay(4500); // wait min 4.1ms
   
    // second try
    write4bits(this, 0x03 << 4);
    delay(4500); // wait min 4.1ms
   
    // third go!
    write4bits(this, 0x03 << 4); 
    delay(150);
   
    // finally, set to 4-bit interface
    write4bits(this, 0x02 << 4); 


	// set # lines, font size, etc.
	command(this, LCD_FUNCTIONSET | this->_displayfunction);  
	
	// turn the display on with no cursor or blinking default
	this->_displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
	LC_I2C_display(this);
	
	// clear it off
	LC_I2C_clear(this);
	
	// Initialize to default text direction (for roman languages)
	this->_displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
	
	// set the entry mode
	command(this, LCD_ENTRYMODESET | this->_displaymode);
	
	LC_I2C_home(this);
  
}

/********** high level commands, for the user! */
void LC_I2C_clear(LiquidCrystal_I2C *this){
	command(this, LCD_CLEARDISPLAY);// clear display, set cursor position to zero
	delay(2000*1000);  // this command takes a long time!
    if (this->_oled) LC_I2C_setCursor(this, 0,0);
}

void LC_I2C_home(LiquidCrystal_I2C *this){
	command(this, LCD_RETURNHOME);  // set cursor position to zero
	delay(2000);  // this command takes a long time!
}

void LC_I2C_setCursor(LiquidCrystal_I2C *this, uint8_t col, uint8_t row){
	int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
	if ( row > this->_numlines ) {
		row = this->_numlines-1;    // we count rows starting w/0
	}
	command(this, LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LC_I2C_print(LiquidCrystal_I2C *this, const char *str) {
	//int size = strlen(str);
	uint8_t *buffer = (uint8_t*)str;
	while (*buffer != '\0') {
		LC_I2C_write(this, *(buffer++));
	}
}

void LC_I2C_printByte(LiquidCrystal_I2C *this, uint8_t byte) {
	LC_I2C_write(this, byte);
}


// Turn the display on/off (quickly)
void LC_I2C_noDisplay(LiquidCrystal_I2C *this) {
	this->_displaycontrol &= ~LCD_DISPLAYON;
	command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}
void LC_I2C_display(LiquidCrystal_I2C *this) {
	this->_displaycontrol |= LCD_DISPLAYON;
	command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

// Turns the underline cursor on/off
void LC_I2C_noCursor(LiquidCrystal_I2C *this) {
	this->_displaycontrol &= ~LCD_CURSORON;
	command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}
void LC_I2C_cursor(LiquidCrystal_I2C *this) {
	this->_displaycontrol |= LCD_CURSORON;
	command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

// Turn on and off the blinking cursor
void LC_I2C_noBlink(LiquidCrystal_I2C *this) {
	this->_displaycontrol &= ~LCD_BLINKON;
	command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}
void LC_I2C_blink(LiquidCrystal_I2C *this) {
	this->_displaycontrol |= LCD_BLINKON;
	command(this, LCD_DISPLAYCONTROL | this->_displaycontrol);
}

// These commands scroll the display without changing the RAM
void LC_I2C_scrollDisplayLeft(LiquidCrystal_I2C *this) {
	command(this, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}
void LC_I2C_scrollDisplayRight(LiquidCrystal_I2C *this) {
	command(this, LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

// These commands scroll the cursor without changing the RAM
void LC_I2C_scrollCursorLeft(LiquidCrystal_I2C *this) {
	command(this, LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVELEFT);
}
void LC_I2C_scrollCursorRight(LiquidCrystal_I2C *this) {
	command(this, LCD_CURSORSHIFT | LCD_CURSORMOVE | LCD_MOVERIGHT);
}

// This is for text that flows Left to Right
void LC_I2C_leftToRight(LiquidCrystal_I2C *this) {
	this->_displaymode |= LCD_ENTRYLEFT;
	command(this, LCD_ENTRYMODESET | this->_displaymode);
}

// This is for text that flows Right to Left
void LC_I2C_rightToLeft(LiquidCrystal_I2C *this) {
	this->_displaymode &= ~LCD_ENTRYLEFT;
	command(this, LCD_ENTRYMODESET | this->_displaymode);
}

// This will 'right justify' text from the cursor
void LC_I2C_autoscroll(LiquidCrystal_I2C *this) {
	this->_displaymode |= LCD_ENTRYSHIFTINCREMENT;
	command(this, LCD_ENTRYMODESET | this->_displaymode);
}

// This will 'left justify' text from the cursor
void LC_I2C_noAutoscroll(LiquidCrystal_I2C *this) {
	this->_displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
	command(this, LCD_ENTRYMODESET | this->_displaymode);
}

// Allows us to fill the first 8 CGRAM locations
// with custom characters
void LC_I2C_createChar(LiquidCrystal_I2C *this, uint8_t location, uint8_t charmap[]) {
	location &= 0x7; // we only have 8 locations 0-7
	command(this, LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
		LC_I2C_write(this, charmap[i]);
	}
}

//createChar with PROGMEM input
/*void LC_I2C_createChar(uint8_t location, const char *charmap) {
	location &= 0x7; // we only have 8 locations 0-7
	command(LCD_SETCGRAMADDR | (location << 3));
	for (int i=0; i<8; i++) {
	    	write(pgm_read_byte_near(charmap++));
	}
}*/

// Turn the (optional) backlight off/on
void LC_I2C_noBacklight(LiquidCrystal_I2C *this) {
	this->_backlightval=LCD_NOBACKLIGHT;
	expanderWrite(this, 0);
}

void LC_I2C_backlight(LiquidCrystal_I2C *this) {
	this->_backlightval=LCD_BACKLIGHT;
	expanderWrite(this, 0);
}



/*********** mid level commands, for sending data/cmds */

static inline void command(LiquidCrystal_I2C *this, uint8_t value) {
	send(this, value, 0);
}





// Alias functions

/*void LC_I2C_cursor_on(){
	cursor();
}

void LC_I2C_cursor_off(){
	noCursor();
}

void LC_I2C_blink_on(){
	blink();
}

void LC_I2C_blink_off(){
	noBlink();
}

void LC_I2C_load_custom_character(uint8_t char_num, uint8_t *rows){
		createChar(char_num, rows);
}

void LC_I2C_setBacklight(uint8_t new_val){
	if(new_val){
		backlight();		// turn backlight on
	}else{
		noBacklight();		// turn backlight off
	}
}

void LC_I2C_printstr(const char c[]){
	//This function is not identical to the function used for "real" I2C displays
	//it's here so the user sketch doesn't have to be changed 
	print(c);
}
*/

// unsupported API functions
/*#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void LC_I2C_off(){}
void LC_I2C_on(){}
void LC_I2C_setDelay (int cmdDelay,int charDelay) {}
uint8_t LC_I2C_status(){return 0;}
uint8_t LC_I2C_keypad (){return 0;}
uint8_t LC_I2C_init_bargraph(uint8_t graphtype){return 0;}
void LC_I2C_draw_horizontal_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_col_end){}
void LC_I2C_draw_vertical_graph(uint8_t row, uint8_t column, uint8_t len,  uint8_t pixel_row_end){}
void LC_I2C_setContrast(uint8_t new_val){}
#pragma GCC diagnostic pop
*/	
