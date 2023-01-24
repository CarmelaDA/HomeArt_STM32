/*
 * ESP8266_HAL.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 */


#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_

// Vectores Fragmentos
char vSeg[2];
char vIlum[27];
char vVent[5];
char vExt[7];
char vTemp[5];
char vSal[6];
char vCoc[2];
char vBan[2];
char vDor[4];
char vOfi[12];
char vGar[2];
char vHuer[2];
char vAj[16];
int readDHT;

void ESP_Init (char *SSID, char *PASSWD);

void serverStart (void);

void ESP_clearBuffer(void);

uint8_t string_compare(char array1[], char array2[], uint16_t length);

int string_contains(char bufferArray[], char searchedString[], uint16_t length);

void ESP_messageHandler(void);



#endif /* INC_ESP8266_HAL_H_ */
