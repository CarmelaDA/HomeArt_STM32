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
#define B_Stop_Pin GPIO_PIN_2
#define B_Stop_GPIO_Port GPIOE
#define S_Int_Pin GPIO_PIN_3
#define S_Int_GPIO_Port GPIOE
#define S_Int_EXTI_IRQn EXTI3_IRQn
#define Fin_Parcela_Pin GPIO_PIN_4
#define Fin_Parcela_GPIO_Port GPIOE
#define Fin_Parcela_EXTI_IRQn EXTI4_IRQn
#define Fin_Garaje_Pin GPIO_PIN_6
#define Fin_Garaje_GPIO_Port GPIOE
#define Fin_Garaje_EXTI_IRQn EXTI9_5_IRQn
#define WiFi_OK_Pin GPIO_PIN_0
#define WiFi_OK_GPIO_Port GPIOC
#define Lluvia_Pin GPIO_PIN_1
#define Lluvia_GPIO_Port GPIOC
#define S_Parcela_Pin GPIO_PIN_0
#define S_Parcela_GPIO_Port GPIOA
#define S_Garaje_Pin GPIO_PIN_1
#define S_Garaje_GPIO_Port GPIOA
#define LDR_Pin GPIO_PIN_4
#define LDR_GPIO_Port GPIOA
#define S_Dormitorio_Pin GPIO_PIN_7
#define S_Dormitorio_GPIO_Port GPIOA
#define DC_Salon_1_Pin GPIO_PIN_4
#define DC_Salon_1_GPIO_Port GPIOC
#define DC_Salon_2_Pin GPIO_PIN_5
#define DC_Salon_2_GPIO_Port GPIOC
#define HW_390_Pin GPIO_PIN_0
#define HW_390_GPIO_Port GPIOB
#define DHT22_Pin GPIO_PIN_7
#define DHT22_GPIO_Port GPIOE
#define DHT11_Pin GPIO_PIN_8
#define DHT11_GPIO_Port GPIOE
#define S_Tendedero_Pin GPIO_PIN_10
#define S_Tendedero_GPIO_Port GPIOB
#define Riego_Pin GPIO_PIN_13
#define Riego_GPIO_Port GPIOB
#define Peltier_Pin GPIO_PIN_15
#define Peltier_GPIO_Port GPIOB
#define S_Oficina_Pin GPIO_PIN_8
#define S_Oficina_GPIO_Port GPIOC
#define S_Salon_Pin GPIO_PIN_9
#define S_Salon_GPIO_Port GPIOC
#define L_GamingG_Pin GPIO_PIN_8
#define L_GamingG_GPIO_Port GPIOA
#define L_GamingR_Pin GPIO_PIN_9
#define L_GamingR_GPIO_Port GPIOA
#define L_GamingB_Pin GPIO_PIN_10
#define L_GamingB_GPIO_Port GPIOA
#define L_Fregadero_Pin GPIO_PIN_15
#define L_Fregadero_GPIO_Port GPIOA
#define L_Cocina_Pin GPIO_PIN_10
#define L_Cocina_GPIO_Port GPIOC
#define L_Garaje_Pin GPIO_PIN_11
#define L_Garaje_GPIO_Port GPIOC
#define L_Tendedero_Pin GPIO_PIN_12
#define L_Tendedero_GPIO_Port GPIOC
#define L_Recibidor_Pin GPIO_PIN_0
#define L_Recibidor_GPIO_Port GPIOD
#define L_Comedor_Pin GPIO_PIN_1
#define L_Comedor_GPIO_Port GPIOD
#define L_Jardin_Pin GPIO_PIN_2
#define L_Jardin_GPIO_Port GPIOD
#define L_Sala_Pin GPIO_PIN_3
#define L_Sala_GPIO_Port GPIOD
#define L_Porche_Pin GPIO_PIN_4
#define L_Porche_GPIO_Port GPIOD
#define L_Ambiente_Pin GPIO_PIN_5
#define L_Ambiente_GPIO_Port GPIOD
#define L_Bano_Pin GPIO_PIN_6
#define L_Bano_GPIO_Port GPIOD
#define L_Dormitorio_Pin GPIO_PIN_7
#define L_Dormitorio_GPIO_Port GPIOD
#define L_Espejo_Pin GPIO_PIN_3
#define L_Espejo_GPIO_Port GPIOB
#define L_Izquierda_Pin GPIO_PIN_4
#define L_Izquierda_GPIO_Port GPIOB
#define L_Oficina_Pin GPIO_PIN_5
#define L_Oficina_GPIO_Port GPIOB
#define L_Derecha_Pin GPIO_PIN_6
#define L_Derecha_GPIO_Port GPIOB
#define L_TV_Pin GPIO_PIN_7
#define L_TV_GPIO_Port GPIOB
#define B_Timbre_Pin GPIO_PIN_0
#define B_Timbre_GPIO_Port GPIOE
#define B_Timbre_EXTI_IRQn EXTI0_IRQn
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
