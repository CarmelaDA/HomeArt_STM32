/*
 * LED.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include "LED.h"


int rgb_value(char i, char j, char k){

	int a, b, c;

	if(i == '0') a = 0;
	else if(i == '1') a = 100;
	else if(i == '2') a = 200;

	if(j == '0') b = 0;
	else if(j == '1') b = 10;
	else if(j == '2') b = 20;
	else if(j == '3') b = 30;
	else if(j == '4') b = 40;
	else if(j == '5') b = 50;
	else if(j == '6') b = 60;
	else if(j == '7') b = 70;
	else if(j == '8') b = 80;
	else if(j == '9') b = 90;

	if(k == '0') c = 0;
	else if(k == '1') c = 1;
	else if(k == '2') c = 2;
	else if(k == '3') c = 3;
	else if(k == '4') c = 4;
	else if(k == '5') c = 5;
	else if(k == '6') c = 6;
	else if(k == '7') c = 7;
	else if(k == '8') c = 8;
	else if(k == '9') c = 9;

	return a+b+c;
}

void auto_light(int i){

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
}
