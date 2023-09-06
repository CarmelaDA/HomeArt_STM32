/*
 * SG90.c
 *
 * Creado por Carmen Díez Almoguera, 2023
 *
 */


#include "SG90.h"
#include "ESP8266_HAL.h"

extern TIM_HandleTypeDef htim2;
extern UART_HandleTypeDef huart6;


void actParcelRFID(){

	// Abrir S_Parcela
	while(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) != 0) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 86);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 94); 	// Rebote
	HAL_Delay(200);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); 	// Parar S_Parcela

	HAL_Delay(5000);

	// Cerrar S_Parcela
	while(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) != 0) __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 94);

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 86); 	// Rebote
	HAL_Delay(200);
	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); 	// Parar S_Parcela

}








