/*
 * SG90.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include <SG90.h>
#include "ESP8266_HAL.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart6;


void actParcelaRFID(){

	while(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == 1) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 91); // Abrir S_Parcela

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89); // Rebote
	HAL_Delay(1000);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // Parar S_Parcela

	HAL_Delay(5000);

	while(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == 1) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89); // Cerrar S_Parcela

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 91); // Rebote
	HAL_Delay(1000);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // Parar S_Parcela

}








