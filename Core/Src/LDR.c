/*
 * LDR.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include "LDR.h"
#include "ESP8266_HAL.h"

void ldr(uint16_t value){


	if (vLight[26] == '1'){

		if(value<90) {
			vLight[23] = 'x';		// Jardín
			vOutside[3] = 'x';
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);
			vLight[24] = 'x';		// Porche
			vOutside[2] = 'x';
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);
			vLight[25] = 'x';		// Tendedero
			vOutside[1] = 'x';
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);
		}
		else{
			vLight[23] = 'x';		// Jardín
			vOutside[3] = 'x';
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET);
			vLight[24] = 'x';		// Porche
			vOutside[2] = 'x';
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET);
			vLight[25] = 'x';		// Tendedero
			vOutside[1] = 'x';
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET);
		}
	}
}








