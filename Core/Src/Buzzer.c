/*
 * Buzzer.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */

#include <Buzzer.h>
#include "ESP8266_HAL.h"

extern TIM_HandleTypeDef htim4;


void playTimbre(){

	uint8_t tone;

	tone = 25;
	__HAL_TIM_SET_AUTORELOAD(&htim4, tone*2);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tone);
	HAL_Delay(300);

	tone = 40;
	__HAL_TIM_SET_AUTORELOAD(&htim4, tone*2);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tone);
	HAL_Delay(800);

	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
}


void playAlarma(){

	uint8_t tone;

	for(tone = 40; tone >= 10; tone = tone-10){
		__HAL_TIM_SET_AUTORELOAD(&htim4, tone*2);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tone);
		HAL_Delay(300);
	}

	__HAL_TIM_SET_AUTORELOAD(&htim4, 80);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 40);
}
