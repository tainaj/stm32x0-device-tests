/*-------------------------------------------------*/
/* Title: 		button.c */
/* Author: 		Joel Taina */
/* Date:		8/22/2020 */
/* Description:	Button module */
/* Peripherals: GPIO */
/*-------------------------------------------------*/

#include "button.h"

/*-----------------------------------------------------*/
/* Private entities */
/*-----------------------------------------------------*/

static struct IOinputDetail inputs[INPUT_CNT];

/*-----------------------------------------------------*/
/* Public functions (Button API) */
/*-----------------------------------------------------*/

void initInputs(int *input_n, int *output_n, int cnt) {
	for (int i=0; i < cnt; i++) {
		inputs[i].name = Input;
		inputs[i].inputPin =  (1 << input_n[i]);
		inputs[i].outputPin = (1 << output_n[i]);
		inputs[i].pressHistory = 0x0;
		inputs[i].prevState = ps_Released;
	}
}

int updateInputs(GPIO_TypeDef *GPIOx, uint16_t current_state) {
	/* (1) Add instantaneous state of input (1: ON. 0: OFF) */
	/* (2) Recognize button press after 8-bit confirmation */
	/* (3) Recognize button release after 8-bit confirmation */
	/* (4) Update return flag (button release) */

	int flags = INPUT_NONE;

    for (int i = 0; i < INPUT_CNT; i++) {
        inputs[i].pressHistory <<= 1;				/* (1) */
        if (current_state & inputs[i].inputPin) {
            inputs[i].pressHistory |= 0x1;
        }
    }
    for (int i = 0; i < INPUT_CNT; i++) {
        if (inputs[i].pressHistory == 0xff) {		/* (2) */
            if (inputs[i].prevState == ps_Released) {
                inputs[i].prevState = ps_Pressed;
                GPIOx->ODR |= inputs[i].outputPin;
            }
        }
        else if (inputs[i].pressHistory == 0x00) {	/* (3) */
            if (inputs[i].prevState == ps_Pressed) {
                 inputs[i].prevState = ps_Released;
                GPIOx->ODR &= ~(inputs[i].outputPin);

                flags |= INPUT_I << i;				/* (4) */
            }
        }
    }

    return flags;
}
