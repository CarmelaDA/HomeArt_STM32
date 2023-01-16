/*
 * DHT11.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */

#include "stm32f4xx_hal.h"
#include "DHT11.h"

#define DHT11_PORT GPIOE
#define DHT11_PIN GPIO_PIN_8

uint8_t RH_byte1, RH_byte2, Temp_byte1, Temp_byte2; // Rh_byte2 y Temp_byte2 en DHT11 son nulos debido a su presición
uint16_t SUM, RH, TEMP;
uint16_t DHT11_presence = 0;

void DHT11_Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP; // Cambiar entre NOPULL y PULLUP si no funciona
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

void DHT11_Start(void){

	DHT11_Set_Pin_Output(DHT11_PORT, DHT11_PIN);  // set the pin as output
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 1);   // pull the pin low

	delay(1000);
	HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 0);

	delay(18000);   // wait for 18ms

    HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, 1);   // pull the pin high
    delay(20);   // wait for 20us
    DHT11_Set_Pin_Input(DHT11_PORT, DHT11_PIN);    // set as input
}

uint8_t DHT11_Check_Response(void){

	uint8_t Response = 0;
	delay (40);
	if (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
	{
		delay (80);

		if ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)))
			Response = 1;

		else
			Response = -1;
	}
	//for(;HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN););
	while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));   // wait for the pin to go low

	return Response;
}

uint8_t DHT11_Read(void){

	uint8_t i,j;
	for (j=0;j<8;j++)
	{
		while (!(HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));   // wait for the pin to go high
		delay (40);   // wait for 40 us
		if (!(HAL_GPIO_ReadPin (DHT11_PORT, DHT11_PIN)))   // if the pin is low
		{
			i&= ~(1<<(7-j));   // write 0
		}
		else i|= (1<<(7-j));  // if the pin is high, write 1
		//for(;HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN);); // Sustituto del WHILE (anti bloqueo)
		while ((HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)));  // wait for the pin to go low
	}
	return i;
}

void DHT11_getData(DHT11_DataTypedef *DHT_Data){

	  DHT11_Start();

	  DHT11_presence = DHT11_Check_Response();

	  RH_byte1 = DHT11_Read();
	  RH_byte2 = DHT11_Read();
	  Temp_byte1 = DHT11_Read();
	  Temp_byte2 = DHT11_Read();

	  SUM = DHT11_Read();

	  RH = RH_byte1;
	  TEMP = Temp_byte1;

	  DHT_Data->Temperature = Temp_byte1;
	  DHT_Data->Humidity = RH_byte1;
}

