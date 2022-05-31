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
#define STOP_Pin GPIO_PIN_2
#define STOP_GPIO_Port GPIOE
#define STOP_EXTI_IRQn EXTI2_IRQn
#define S_Int_Pin GPIO_PIN_3
#define S_Int_GPIO_Port GPIOE
#define S_Int_EXTI_IRQn EXTI3_IRQn
#define Abierto_Pin GPIO_PIN_4
#define Abierto_GPIO_Port GPIOE
#define Abierto_EXTI_IRQn EXTI4_IRQn
#define Cerrado_Pin GPIO_PIN_5
#define Cerrado_GPIO_Port GPIOE
#define Cerrado_EXTI_IRQn EXTI9_5_IRQn
#define WiFi_OK_Pin GPIO_PIN_0
#define WiFi_OK_GPIO_Port GPIOC
#define P_Parcela_Pin GPIO_PIN_0
#define P_Parcela_GPIO_Port GPIOA
#define P_Garaje_Pin GPIO_PIN_1
#define P_Garaje_GPIO_Port GPIOA
#define V_Salon_Pin GPIO_PIN_6
#define V_Salon_GPIO_Port GPIOA
#define V_Dormitorio_Pin GPIO_PIN_7
#define V_Dormitorio_GPIO_Port GPIOA
#define V_Oficina_Pin GPIO_PIN_0
#define V_Oficina_GPIO_Port GPIOB
#define Tendedero_Pin GPIO_PIN_10
#define Tendedero_GPIO_Port GPIOB
#define Zumbador_Pin GPIO_PIN_12
#define Zumbador_GPIO_Port GPIOD
#define Z_Activo_Pin GPIO_PIN_15
#define Z_Activo_GPIO_Port GPIOD
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
#define Timbre_Pin GPIO_PIN_0
#define Timbre_GPIO_Port GPIOE
#define Timbre_EXTI_IRQn EXTI0_IRQn
#define S_Ext_Pin GPIO_PIN_1
#define S_Ext_GPIO_Port GPIOE
#define S_Ext_EXTI_IRQn EXTI1_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
