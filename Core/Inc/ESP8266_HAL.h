/*
 * ESP8266_HAL.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 */


#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_


void ESP_Init (char *SSID, char *PASSWD);

void serverStart (void);

void ESP_clearBuffer(void);

uint8_t string_compare(char array1[], char array2[], uint16_t length);

int string_contains(char bufferArray[], char searchedString[], uint16_t length);

void ESP_messageHandler(void);



#endif /* INC_ESP8266_HAL_H_ */
