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

	  char textrc[16] = "";

	  char coche[3] = "247";
	  char moto[3] = "355";

	  int familia = 0;

	  for(int i=0; i<16; i++) textrc[i] = ' ';

	  HAL_UART_Receive(&huart3, (uint8_t *) textrc, 16, 100);
	  HAL_UART_Transmit(&huart6, (uint8_t *) textrc, 16, HAL_MAX_DELAY);

	  if(textrc[16] != ' '){
		  for(int i = 0; i<3; i++){
			  familia = 1;
			  if(textrc[i+13] != coche[i]){
				  familia = 0;
				  i = 2;
			  }
		  }

		  if(!familia){
			  for(int i = 0; i<3; i++){
				  familia = 1;
				  if(textrc[13+i] != moto[i]){
					  familia = 0;
					  i = 2;
				  }
			  }
		  }
	  }

	  if(textrc[15] != ' '){
		  if(familia) return 1;
		  else return 0;
	  }
	  else return 0;
}








