/*
 * ESP8266_HAL.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 */


#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_


// Arrays
char vSecurity[2];
char vLight[27];
char vWindow[5];
char vOutside[7];
char vWeather[5];
char vLiving[6];
char vKitchen[2];
char vBath[2];
char vBedroom[4];
char vOffice[12];
char vGarage[2];
char vGarden[2];
char vSettings[16];
int readDHT;

void ESP_Init (char *SSID, char *PASSWD);

void serverStart (void);

void ESP_clearBuffer(void);

uint8_t string_compare(char array1[], char array2[], uint16_t length);

int string_contains(char bufferArray[], char searchedString[], uint16_t length);

void ESP_messageHandler(void);



#endif /* INC_ESP8266_HAL_H_ */
