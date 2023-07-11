/*
 * RFID.c
 *
 * Creado por Carmen Díez Almoguera, 2023
 *
 */


#include "RFID.h"
//#include "ESP8266_HAL.h"

extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;


int readRFID(){

	  char vehicle[16] = "";

	  char car[3] = "247";
	  char motorcycle[3] = "355";

	  int family = 0;

	  for(int i=0; i<16; i++) vehicle[i] = '_';

	  HAL_UART_Receive(&huart3, (uint8_t *) vehicle, 16, 100);

	  if (vehicle[15] == '_') return 2;

	  else {
		  for(int i = 0; i<3; i++){
			  family = 1;
			  if(vehicle[i+13] != car[i]){
				  family = 0;
				  i = 2;
			  }
		  }

		  if(!family){
			  for(int i = 0; i<3; i++){
				  family = 1;
				  if(vehicle[13+i] != motorcycle[i]){
					  family = 0;
					  i = 2;
				  }
			  }
		  }
	  }

	  return family;
}








