/*
 * DHT22.h
 *
 * Modificado por Carmen Díez Almoguera, 2023
 *
 */

#ifndef DHT22_H_
#define DHT22_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"


typedef struct{

	float Temperature;
	float Humidity;

}DHT22_DataTypedef;

void delay(uint16_t time);

void DHT22_Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void DHT22_Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void DHT22_Start(void);

uint8_t DHT22_Check_Response(void);

uint8_t DHT22_Read(void);

void DHT22_getData(DHT22_DataTypedef *DHT_Data);


#endif /* INC_DHT22_H_ */
