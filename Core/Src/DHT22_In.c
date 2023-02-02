/*
 * DHT22.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */

#include <DHT22.h>
#include "stm32f4xx_hal.h"

#define DHT22_PORT GPIOE
#define DHT22_PIN GPIO_PIN_7

uint8_t RH_byte1, RH_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM, RH, TEMP;
uint16_t DHT22_presence = 0;


void DHT22_Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT22_Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP; // change to PULLUP if it does not work
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT22_Start(void){

	DHT22_Set_Pin_Output(DHT22_PORT, DHT22_PIN); // set the pin as output
	HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 1);   // pull the pin low
	HAL_Delay(1200);   // wait for > 1ms
	HAL_GPIO_WritePin(DHT22_PORT, DHT22_PIN, 0);
	delay(18000);
	HAL_GPIO_WritePin (DHT22_PORT, DHT22_PIN, 1);   // pull the pin high
	delay (30);   // wait for 30us

	DHT22_Set_Pin_Input(DHT22_PORT, DHT22_PIN);   // set as input
}

uint8_t DHT22_Check_Response(void){

	DHT22_Set_Pin_Input(DHT22_PORT, DHT22_PIN);   // set as input
	uint8_t Response = 0;
	delay (40);  // wait for 40us
	if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) // if the pin is low
	{
		delay (80);   // wait for 80us

		if ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN))) Response = 1;  // if the pin is high, response is ok
		else Response = -1;
	}

	while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)));   // wait for the pin to go low
	return Response;
}

uint8_t DHT22_Read(void){

	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)));   // wait for the pin to go high
		delay (40);   // wait for 40 us

		if (!(HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		while ((HAL_GPIO_ReadPin (DHT22_PORT, DHT22_PIN)));  // wait for the pin to go low
	}

	return i;
}

void DHT22_getData(DHT22_DataTypedef *DHT_Data){

	  DHT22_Start();
	  DHT22_presence = DHT22_Check_Response();

	  RH_byte1 = DHT22_Read();
	  RH_byte2 = DHT22_Read();
	  Temp_byte1 = DHT22_Read();
	  Temp_byte2 = DHT22_Read();

	  SUM = DHT22_Read();

	  RH = ((RH_byte1<<8)|RH_byte2);
	  TEMP = ((Temp_byte1<<8)|Temp_byte2);

	  DHT_Data->Temperature = (float) (TEMP/10.0);
	  DHT_Data->Humidity = (float) (RH/10.0);
}

