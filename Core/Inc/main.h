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
#define S_In_Pin GPIO_PIN_3
#define S_In_GPIO_Port GPIOE
#define S_In_EXTI_IRQn EXTI3_IRQn
#define Parcel_Limit_Pin GPIO_PIN_4
#define Parcel_Limit_GPIO_Port GPIOE
#define Parcel_Limit_EXTI_IRQn EXTI4_IRQn
#define Garage_Limit_Pin GPIO_PIN_6
#define Garage_Limit_GPIO_Port GPIOE
#define Garage_Limit_EXTI_IRQn EXTI9_5_IRQn
#define B_People_Bell_Pin GPIO_PIN_13
#define B_People_Bell_GPIO_Port GPIOC
#define B_People_Bell_EXTI_IRQn EXTI15_10_IRQn
#define B_Stop_Pin GPIO_PIN_15
#define B_Stop_GPIO_Port GPIOC
#define B_Stop_EXTI_IRQn EXTI15_10_IRQn
#define WiFi_OK_Pin GPIO_PIN_0
#define WiFi_OK_GPIO_Port GPIOC
#define Rain_Pin GPIO_PIN_1
#define Rain_GPIO_Port GPIOC
#define Parcel_Servo_Pin GPIO_PIN_0
#define Parcel_Servo_GPIO_Port GPIOA
#define Garage_Servo_Pin GPIO_PIN_1
#define Garage_Servo_GPIO_Port GPIOA
#define LDR_Pin GPIO_PIN_4
#define LDR_GPIO_Port GPIOA
#define Bedroom_Servo_Pin GPIO_PIN_7
#define Bedroom_Servo_GPIO_Port GPIOA
#define HW_390_Pin GPIO_PIN_0
#define HW_390_GPIO_Port GPIOB
#define DHT22_In_Pin GPIO_PIN_7
#define DHT22_In_GPIO_Port GPIOE
#define DHT22_Out_Pin GPIO_PIN_8
#define DHT22_Out_GPIO_Port GPIOE
#define Awning_Servo_Pin GPIO_PIN_10
#define Awning_Servo_GPIO_Port GPIOB
#define Water_Pin GPIO_PIN_13
#define Water_GPIO_Port GPIOB
#define Heat_Pin GPIO_PIN_15
#define Heat_GPIO_Port GPIOB
#define Office_Servo_Pin GPIO_PIN_8
#define Office_Servo_GPIO_Port GPIOC
#define Living_Servo_Pin GPIO_PIN_9
#define Living_Servo_GPIO_Port GPIOC
#define L_GamingG_Pin GPIO_PIN_8
#define L_GamingG_GPIO_Port GPIOA
#define L_GamingR_Pin GPIO_PIN_9
#define L_GamingR_GPIO_Port GPIOA
#define L_GamingB_Pin GPIO_PIN_10
#define L_GamingB_GPIO_Port GPIOA
#define L_Sink_Pin GPIO_PIN_15
#define L_Sink_GPIO_Port GPIOA
#define L_Kitchen_Pin GPIO_PIN_10
#define L_Kitchen_GPIO_Port GPIOC
#define L_Garage_Pin GPIO_PIN_11
#define L_Garage_GPIO_Port GPIOC
#define L_ClothesLine_Pin GPIO_PIN_12
#define L_ClothesLine_GPIO_Port GPIOC
#define L_Hall_Pin GPIO_PIN_0
#define L_Hall_GPIO_Port GPIOD
#define L_Dining_Pin GPIO_PIN_1
#define L_Dining_GPIO_Port GPIOD
#define L_Garden_Pin GPIO_PIN_2
#define L_Garden_GPIO_Port GPIOD
#define L_Living_Pin GPIO_PIN_3
#define L_Living_GPIO_Port GPIOD
#define L_Porch_Pin GPIO_PIN_4
#define L_Porch_GPIO_Port GPIOD
#define L_Relax_Pin GPIO_PIN_5
#define L_Relax_GPIO_Port GPIOD
#define L_Bathroom_Pin GPIO_PIN_6
#define L_Bathroom_GPIO_Port GPIOD
#define L_Bedroom_Pin GPIO_PIN_7
#define L_Bedroom_GPIO_Port GPIOD
#define L_Mirror_Pin GPIO_PIN_3
#define L_Mirror_GPIO_Port GPIOB
#define L_Left_Pin GPIO_PIN_4
#define L_Left_GPIO_Port GPIOB
#define L_Office_Pin GPIO_PIN_5
#define L_Office_GPIO_Port GPIOB
#define L_Right_Pin GPIO_PIN_6
#define L_Right_GPIO_Port GPIOB
#define L_TV_Pin GPIO_PIN_7
#define L_TV_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB
#define B_Vehicle_Bell_Pin GPIO_PIN_0
#define B_Vehicle_Bell_GPIO_Port GPIOE
#define B_Vehicle_Bell_EXTI_IRQn EXTI0_IRQn
#define S_Out_Pin GPIO_PIN_1
#define S_Out_GPIO_Port GPIOE
#define S_Out_EXTI_IRQn EXTI1_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
