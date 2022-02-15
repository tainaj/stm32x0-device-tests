/*-------------------------------------------------*/
/* Title: 		button.h */
/* Author: 		Joel Taina */
/* Date:		8/22/2020 */
/* Description:	Header for button module. */
/* Peripherals: GPIO */
/*-------------------------------------------------*/

#ifndef BUTTON_H_
#define BUTTON_H_

#include "global.h"

/*-----------------------------------------------------*/
/* Defines */
/*-----------------------------------------------------*/

#define INPUT_CNT 3 		// Number of buttons (modify src init)
#define INPUT_NONE	0x00
#define INPUT_I		0x01

/*-----------------------------------------------------*/
/* Enums for button names and states */
/*-----------------------------------------------------*/

// Official button state
typedef enum {
	ps_Pressed = 0,
	ps_Released
} ePrevState;

// Button names
typedef enum {
	Input      = 0,
    L_Button,
	R_Button,
	Hall_Sensor
} eSystemInput;

/*-----------------------------------------------------*/
/* Structs for button management */
/*-----------------------------------------------------*/

// Button metadata
struct IOinputDetail {
    eSystemInput name;
    uint32_t inputPin;
    uint32_t outputPin;
    uint8_t pressHistory;
    ePrevState prevState;
};

/*-----------------------------------------------------*/
/* Public functions (Button module API) */
/*-----------------------------------------------------*/

extern void initInputs(int *input_n, int *output_n, int cnt);
extern int updateInputs(GPIO_TypeDef *GPIOx, uint16_t current_state);

#endif /* BUTTON_H_ */
