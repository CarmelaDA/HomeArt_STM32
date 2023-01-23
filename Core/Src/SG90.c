/*
 * SG90.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include <SG90.h>
#include "ESP8266_HAL.h"

extern TIM_HandleTypeDef htim2;

void actParcelaRFID(){

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 92); // Abrir S_Parcela

	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == 0){
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89); // Rebote
		HAL_Delay(1000);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // Parar S_Parcela
	}

	HAL_Delay(5000);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89); // Cerrar S_Parcela

	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == 0){
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 92); // Rebote
		HAL_Delay(1000);
		__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // Parar S_Parcela
	}
}








