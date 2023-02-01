/*
 * RFID.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include <RFID.h>
#include "ESP8266_HAL.h"

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;


int readLector(){

	  char vehicle[16] = "";

	  char coche[3] = "247";
	  char moto[3] = "355";

	  int familia = 0;

	  for(int i=0; i<16; i++) vehicle[i] = '_';

	  HAL_UART_Receive(&huart3, (uint8_t *) vehicle, 16, 100);

	  if (vehicle[15] == '_') return 2;

	  else {
		  for(int i = 0; i<3; i++){
			  familia = 1;
			  if(vehicle[i+13] != coche[i]){
				  familia = 0;
				  i = 2;
			  }
		  }

		  if(!familia){
			  for(int i = 0; i<3; i++){
				  familia = 1;
				  if(vehicle[13+i] != moto[i]){
					  familia = 0;
					  i = 2;
				  }
			  }
		  }
	  }

	  return familia;
}








