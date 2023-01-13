/*
 * LDR.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include <LDR.h>
#include "ESP8266_HAL.h"

void ldr(uint16_t valor){


	if (vIlum[26] == '1'){

		if(valor<90) {
			vIlum[23] = 'x';	// Jardín
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);
			vIlum[24] = 'x';	// Porche
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);
			vIlum[25] = 'x';	// Tendedero
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);
		}
		else{
			vIlum[23] = 'x';	// Jardín
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET);
			vIlum[24] = 'x';	// Porche
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET);
			vIlum[25] = 'x';	// Tendedero
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET);
		}
	}
}








