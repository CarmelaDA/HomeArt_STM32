/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define WiFi_OK_Pin GPIO_PIN_0
#define WiFi_OK_GPIO_Port GPIOC
#define L_Porche_Pin GPIO_PIN_10
#define L_Porche_GPIO_Port GPIOC
#define L_Tendedero_Pin GPIO_PIN_11
#define L_Tendedero_GPIO_Port GPIOC
#define L_Garaje_Pin GPIO_PIN_12
#define L_Garaje_GPIO_Port GPIOC
#define L_Jardin_Pin GPIO_PIN_0
#define L_Jardin_GPIO_Port GPIOD
#define Oficina_Pin GPIO_PIN_1
#define Oficina_GPIO_Port GPIOD
#define L_Derecha_Pin GPIO_PIN_2
#define L_Derecha_GPIO_Port GPIOD
#define L_Izquierda_Pin GPIO_PIN_3
#define L_Izquierda_GPIO_Port GPIOD
#define L_Domitorio_Pin GPIO_PIN_4
#define L_Domitorio_GPIO_Port GPIOD
#define L_Espejo_Pin GPIO_PIN_5
#define L_Espejo_GPIO_Port GPIOD
#define L_Bano_Pin GPIO_PIN_6
#define L_Bano_GPIO_Port GPIOD
#define L_Fregadero_Pin GPIO_PIN_7
#define L_Fregadero_GPIO_Port GPIOD
#define L_Cocina_Pin GPIO_PIN_3
#define L_Cocina_GPIO_Port GPIOB
#define L_Recibidor_Pin GPIO_PIN_4
#define L_Recibidor_GPIO_Port GPIOB
#define L_Ambiente_Pin GPIO_PIN_5
#define L_Ambiente_GPIO_Port GPIOB
#define L_Comedor_Pin GPIO_PIN_6
#define L_Comedor_GPIO_Port GPIOB
#define L_Sala_Pin GPIO_PIN_7
#define L_Sala_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/