/*
 * LED.c
 *
 * Creado por Carmen Díez Almoguera, 2023
 *
 */


#include "LED.h"


int rgb_value(char i, char j, char k){

	char rgb[3];
	rgb[0] = i;
	rgb[1] = j;
	rgb[2] = k;

	int val = atoi(rgb) ;

	return val;
}

/*void auto_light(int i){

	if(i){
		//vIlum[23]='1';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);
		//vIlum[24]='1';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);
		//vIlum[25]='1';
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);
	}
	else{
		//vIlum[23]='0';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET);
		//vIlum[24]='0';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET);
		//vIlum[25]='0';
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET);
	}
}*/
