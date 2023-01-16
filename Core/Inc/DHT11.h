/*
 * DHT11.h
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */

#ifndef DHT11_H_
#define DHT11_H_

typedef struct{

	int Temperature;
	int Humidity;

}DHT11_DataTypedef;

void delay(uint16_t time);

void DHT11_Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void DHT11_Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void DHT11_Start(void);

uint8_t DHT11_Check_Response(void);

uint8_t DHT11_Read(void);

void DHT11_getData(DHT11_DataTypedef *DHT_Data);


#endif /* INC_DHT11_H_ */
