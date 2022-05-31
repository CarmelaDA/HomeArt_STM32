/*
 * ESP8266_HAL.c
 *
 * Creado por Carmen Díez Almoguera, 2022
 *
 */


#include <UART_HAL.h>
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "string.h"
#include "action.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

// Definición de los puertos de comunicación
#define WiFi_UART &huart2
#define PC_UART &huart6

char buffer[20];
char buffer_app[60];
#define TOKEN	"fe5g8e2a5f4e85d2e85a7c5"
uint16_t buffer_index;
uint8_t pin = 0;
//int position = 0;

char textrc[110];
char fragment[1];


int r, g, b;

char pine[2];
//, timeout, messageHandlerFlag, watchdog, enable_w;

char *Basic_inclusion = "<!DOCTYPE html> <html>\n<head><meta name=\"viewport\"\
		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n\
		<title>HomeArt Server</title>\n<style>html { font-family: Helvetica; \
		display: inline-block; margin: 0px auto; text-align: center;}\n\
		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\
		h3 {color: #444444;margin-bottom: 50px;}\n.button {display: block;\
		width: 80px;background-color: #1abc9c;border: none;color: white;\
		padding: 13px 30px;text-decoration: none;font-size: 25px;\
		margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n\
		.button-on {background-color: #1abc9c;}\n.button-on:active \
		{background-color: #16a085;}\n.button-off {background-color: #34495e;}\n\
		.button-off:active {background-color: #2c3e50;}\np {font-size: 14px;color: #888;margin-bottom: 10px;}\n\
		</style>\n</head>\n<body>\n<h1>HOMEART SERVER</h1>\n<p>by Carmela</p>";

char *LED_ON = "<p>LED Status: ON</p><a class=\"button button-off\" href=\"/ledoff\">OFF</a>";
char *LED_OFF = "<p>LED Status: OFF</p><a class=\"button button-on\" href=\"/ledon\">ON</a>";
//char *LED_ON = "";
//char *LED_OFF = "";
char *Terminate = "</body></html>";


/*****************************************************************************************************************************************/

void ESP_clearBuffer(void){

	memset(buffer_app, 0, 60);
	buffer_index = 0;
}


void ESP_Init(char *SSID, char *PASSWD){

	ESP_clearBuffer();
	char data[80];

	ringInit();

	HAL_Delay(1000);

	/********** AT **********/

	UART_send("AT\r\n", WiFi_UART);
	while(!(UART_waitFor("OK\r\n", WiFi_UART)));
	UART_send("AT  ---->  OK\n", PC_UART);


	HAL_Delay(2000);


	/********** AT+RST **********/
	UART_send("AT+RST\r\n", WiFi_UART);
	UART_send("\nReseteando ", PC_UART);

	for (int i=0; i<3; i++)
	{
		UART_send(" . ", PC_UART);
		HAL_Delay(1500);
	}


	/********** AT **********/
	UART_send("AT\r\n", WiFi_UART);
	while(!(UART_waitFor("OK\r\n", WiFi_UART)));
	UART_send("\n\nAT  ---->  OK\n\n", PC_UART);


	HAL_Delay(2000);


	/********** AT+CWMODE=3 **********/
	UART_send("AT+CWMODE=3\r\n", WiFi_UART);
	while (!(UART_waitFor("OK\r\n", WiFi_UART)));
	UART_send("CW MODE  ---->  3\n\n", PC_UART);


	/********** AT+CWJAP="SSID","PASSWD" **********/
	UART_send("Conectando con la RED proporcionada ", PC_UART);

	for (int i=0; i<3; i++)
	{
		UART_send(" . ", PC_UART);
		HAL_Delay(1500);
	}

	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	UART_send(data, WiFi_UART);
	while (!(UART_waitFor("WIFI GOT IP\r\n\r\nOK\r\n", WiFi_UART)));
	sprintf (data, "\n\nConectado a  \"%s\"\n\n", SSID);
	UART_send(data,PC_UART);

	//UART_send("AT+CIPSTA?\r\n", WiFi_UART);
	//sprintf (data, "AT+CIPSTA?\r\n");


	/********** AT+CIFSR **********/
	UART_send("AT+CIFSR\r\n", WiFi_UART);
	while (!(UART_waitFor("CIFSR:STAIP,\"", WiFi_UART)));
	while (!(UART_copyUpto("\"",buffer, WiFi_UART)));
	while (!(UART_waitFor("OK\r\n", WiFi_UART)));
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	sprintf (data, "IP :  %s\n\n", buffer);
	UART_send(data, PC_UART);


	/********** AT+CIPMUX=1 **********/
	UART_send("AT+CIPMUX=1\r\n", WiFi_UART);
	while (!(UART_waitFor("AT+CIPMUX=1\r\r\n\r\nOK\r\n", WiFi_UART)));
	UART_send("CIPMUX  ---->  OK\n\n", PC_UART);


	/********** AT+CIPSTART=0**********/
	//UART_send("AT+CIPSTART=\"TCPv6\",\"172.20.10.5\",\"80\"\r\n", WiFi_UART);
	//while (!(UART_waitFor("OK\r\n", WiFi_UART)));
	//UART_send("CIPSTART  ---->  OK\n\n", PC_UART);


	/********** AT+CIPSERVER=1,80 **********/
	UART_send("AT+CIPSERVER=1,80\r\n", WiFi_UART);
	while (!(UART_waitFor("OK\r\n", WiFi_UART)));
	UART_send("CIPSERVER  ---->  OK\n\n", PC_UART);


	/********** FIN **********/
	UART_send("Conectado a la IP\n\n", PC_UART);
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, 1); // CONNECTED
}


int serverSend(char *str, int Link_ID){

	int len = strlen (str);
	char data[80];
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	UART_send(data, WiFi_UART);
	while (!(UART_waitFor(">", WiFi_UART)));
	UART_send(str, WiFi_UART);
	while (!(UART_waitFor("SEND OK", WiFi_UART)));
	sprintf (data, "AT+CIPCLOSE=5\r\n");
	UART_send(data, WiFi_UART);
	while (!(UART_waitFor("OK\r\n", WiFi_UART)));
	return 1;
}


void serverHandle(char *str, int Link_ID){

	char datatosend[1024] = {0};

	if (!(strcmp (str, "/ledon"))){

		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_ON);
		strcat(datatosend, Terminate);
		serverSend(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/ledoff"))){

		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_OFF);
		strcat(datatosend, Terminate);
		serverSend(datatosend, Link_ID);
	}

	else{

		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED_OFF);
		strcat(datatosend, Terminate);
		serverSend(datatosend, Link_ID);
	}
}


void serverStart(void){

	char buftocopyinto[64] = {0};
	char Link_ID;

	while (!(UART_getAfter("+IPD,", 1, &Link_ID, WiFi_UART)));

	Link_ID -= 48;

	while (!(UART_copyUpto(" HTTP/1.1", buftocopyinto, WiFi_UART)));

	if (lookFor("/ledon", buftocopyinto) == 1)
	{
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
		serverHandle("/ledon",Link_ID);
	}

	else if (lookFor("/ledoff", buftocopyinto) == 1)
	{
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		serverHandle("/ledoff",Link_ID);
	}

	else if (lookFor("/favicon.ico", buftocopyinto) == 1);

	else
	{
		//HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
		serverHandle("/ ", Link_ID);
	}
}


uint8_t stringCompare(char array1[], char array2[], uint16_t length){

	 uint16_t comVAR=0, i;
	 for(i=0;i<length;i++){

	   		  if(array1[i]==array2[i])
	   	  		  comVAR++;
	   	  	  else comVAR=0;
	 }

	 if (comVAR==length)
		 	return 1;
	 else 	return 0;
}


int stringContains(char bufferArray[], char searchedString[], uint16_t length){

	uint8_t result=0;
	for(uint16_t i=0; i<length; i++)
	{
		result = stringCompare(&bufferArray[i], &searchedString[0], strlen(searchedString));
		if(result == 1)
			return i;
	}
	return -1;
}


void ESP_messageHandler(void){

	//__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);

	memset(textrc, 0, 100);

	HAL_UART_Receive(&huart2, (uint8_t *)textrc, 100, 100); //(uint8_t *)

	HAL_UART_Transmit(&huart6, (uint8_t *)textrc, 100, HAL_MAX_DELAY);
	UART_send("\n", PC_UART);

	fragment[0] = textrc[25]; // Asignación de Fragmento
	HAL_UART_Transmit(&huart6, (uint8_t *)fragment, 1, HAL_MAX_DELAY);
	UART_send("\n", PC_UART);


	// SEGURIDAD
	if (fragment[0] == 's'){
		UART_send("SEGURIDAD \n", PC_UART);
		vSeg[0] = textrc[28]; // Alarma Interior
		vSeg[1] = textrc[31]; // Alarma Exterior
	}

	// ILUMINACIÓN
	if (fragment[0] == 'i'){
		UART_send("ILUMINACION \n", PC_UART);
		vIlum[0] = textrc[28]; 	// Luz Sala
		vIlum[1] = textrc[31]; 	// Luz Comedor
		vIlum[2] = textrc[34]; 	// Luz Ambiente
		vIlum[3] = textrc[37]; 	// Luz Recibidor
		vIlum[4] = textrc[40]; 	// Luz Cocina
		vIlum[5] = textrc[43]; 	// Luz Fregadero
		vIlum[6] = textrc[46]; 	// Luz Baño
		vIlum[7] = textrc[49]; 	// Luz Espejo
		vIlum[8] = textrc[52]; 	// Luz Dormitorio
		vIlum[9] = textrc[55]; 	// Luz Mesita Izq
		vIlum[10] = textrc[58]; // Luz Mesita Dch
		vIlum[11] = textrc[61]; // Luz Oficina
		vIlum[12] = textrc[64]; // Luz Gaming
		vIlum[13] = textrc[67]; // Luz Rx100
		vIlum[14] = textrc[68]; // Luz Rx10
		vIlum[15] = textrc[69]; // Luz Rx1
		vIlum[16] = textrc[72]; // Luz Gx100
		vIlum[17] = textrc[73]; // Luz Gx10
		vIlum[18] = textrc[74]; // Luz Gx1
		vIlum[19] = textrc[77]; // Luz Bx100
		vIlum[20] = textrc[78]; // Luz Bx10
		vIlum[21] = textrc[79]; // Luz Bx1
		vIlum[22] = textrc[82]; // Luz Garaje
		vIlum[23] = textrc[85]; // Luz Jardín
		vIlum[24] = textrc[88]; // Luz Porche
		vIlum[25] = textrc[91]; // Luz Tendedero


		if(vIlum[0] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, RESET);
		else if(vIlum[0] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, SET);

		if(vIlum[1] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET);
		else if(vIlum[1] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);

		if(vIlum[2] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET);
		else if(vIlum[2] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, SET);

		if(vIlum[3] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET);
		else if(vIlum[3] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);

		if(vIlum[4] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, RESET);
		else if(vIlum[4] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, SET);

		if(vIlum[5] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, RESET);
		else if(vIlum[5] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, SET);

		if(vIlum[6] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, RESET);
		else if(vIlum[6] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, SET);

		if(vIlum[7] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, RESET);
		else if(vIlum[7] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, SET);

		if(vIlum[8] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET);
		else if(vIlum[8] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);

		if(vIlum[9] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, RESET);
		else if(vIlum[9] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, SET);

		if(vIlum[10] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET);
		else if(vIlum[10] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);

		if(vIlum[11] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, RESET);
		else if(vIlum[11] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, SET);

		if(vIlum[12] == '0'){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		}
		else if(vIlum[12] == '1'){

			//uint8_t r, g, b;

			r = rgb_value (vIlum[13],vIlum[14],vIlum[15]);
			g = rgb_value (vIlum[16],vIlum[17],vIlum[18]);
			b = rgb_value (vIlum[19],vIlum[20],vIlum[21]);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, g); // PA8
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, r); // PA9
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, b); // PA10
		}

		if(vIlum[22] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET);
		else if(vIlum[22] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);

		if(vIlum[23] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, RESET);
		else if(vIlum[23] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, SET);

		if(vIlum[24] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, RESET);
		else if(vIlum[24] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, SET);

		if(vIlum[25] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, RESET);
		else if(vIlum[25] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, SET);
	}

	// PUERTAS Y VENTANAS
	if (fragment[0] == 'p'){
		UART_send("PUERTAS Y VENTANAS (PUERTA PARCELA)\n", PC_UART);
		vVent[0] = textrc[28]; // Puerta Parcela
	}
	if (fragment[0] == 'g'){
		UART_send("PUERTAS Y VENTANAS (PUERTA GARAJE)\n", PC_UART);
		vVent[1] = textrc[28]; // Puerta Garaje
	}
	if (fragment[0] == 'l'){
		UART_send("PUERTAS Y VENTANAS (VENTANA SALÓN)\n", PC_UART);
		vVent[2] = textrc[28]; // Ventana Salón
	}
	if (fragment[0] == 'd'){
		UART_send("PUERTAS Y VENTANAS (VENTANA DORMITORIO)\n", PC_UART);
		vVent[3] = textrc[28]; // Ventana Dormitorio
	}
	if (fragment[0] == 'o'){
		UART_send("PUERTAS Y VENTANAS (VENTANA OFICINA)\n", PC_UART);
		vVent[4] = textrc[28]; // Ventana Oficina
	}

	//__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}
