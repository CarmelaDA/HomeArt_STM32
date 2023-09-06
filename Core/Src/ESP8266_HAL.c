/*
 * ESP8266_HAL.c
 *
 * Modificado por Carmen Díez Almoguera, 2023
 *
 */


#include "LED.h"
#include "UART_HAL.h"
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

// Comunication Ports Definition
#define WiFi_UART &huart2
#define PC_UART &huart6

char buffer[20];
char buffer_app[60];
#define TOKEN	"fe5g8e2a5f4e85d2e85a7c5"
uint16_t buffer_index;
uint8_t pin = 0;
//int position = 0;

char textrc[110];
char dataSTM[1];
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
	UART_send("\nAT  ---->  OK\n", PC_UART);


	HAL_Delay(2000);


	/********** AT+RST **********/
	UART_send("AT+RST\r\n", WiFi_UART);
	UART_send("\nResetting ", PC_UART);

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
	UART_send("Connecting ", PC_UART);

	for (int i=0; i<3; i++)
	{
		UART_send(" . ", PC_UART);
		HAL_Delay(1500);
	}

	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	UART_send(data, WiFi_UART);
	while (!(UART_waitFor("WIFI GOT IP\r\n\r\nOK\r\n", WiFi_UART)));
	sprintf (data, "\n\nConnected to \"%s\"\n\n", SSID);
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
	UART_send("Conected to the IP\n\n", PC_UART);
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

	if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0) != 0){

		HAL_UART_Receive(&huart2, (uint8_t *)textrc, 100, 100); //(uint8_t *)

		HAL_UART_Transmit(&huart6, (uint8_t *)textrc, 100, HAL_MAX_DELAY);
		UART_send("\n", PC_UART);

		fragment[0] = textrc[25]; // Fragment assignment
		HAL_UART_Transmit(&huart6, (uint8_t *)fragment, 1, HAL_MAX_DELAY);
		UART_send("\n", PC_UART);
	}


	// DHT22
	if (fragment[0] == 'H'){
		UART_send("LECTURA \n", PC_UART);
		readDHT = textrc[28]; // Orden de Lectura
	}

	// SECURITY
	if (fragment[0] == 's'){
		UART_send("SEGURIDAD \n", PC_UART);
		vSecurity[0] = textrc[28]; // Alarma Interior
		vSecurity[1] = textrc[31]; // Alarma Exterior
	}

	// LIGHTS
	if (fragment[0] == 'i'){
		UART_send("ILUMINACION \n", PC_UART);
		vLight[0] = textrc[28]; 	// Luz Sala
		vLight[1] = textrc[31]; 	// Luz Comedor
		vLight[2] = textrc[34]; 	// Luz Ambiente
		vLight[3] = textrc[37]; 	// Luz Recibidor
		vLight[4] = textrc[40]; 	// Luz Cocina
		vLight[5] = textrc[43]; 	// Luz Fregadero
		vLight[6] = textrc[46]; 	// Luz Baño
		vLight[7] = textrc[49]; 	// Luz Espejo
		vLight[8] = textrc[52]; 	// Luz Dormitorio
		vLight[9] = textrc[55]; 	// Luz Mesita Izq
		vLight[10] = textrc[58]; 	// Luz Mesita Dch
		vLight[11] = textrc[61]; 	// Luz Oficina
		vLight[12] = textrc[64]; 	// Luz Gaming
		vLight[13] = textrc[67]; 	// Luz Rx100
		vLight[14] = textrc[68]; 	// Luz Rx10
		vLight[15] = textrc[69]; 	// Luz Rx1
		vLight[16] = textrc[72]; 	// Luz Gx100
		vLight[17] = textrc[73]; 	// Luz Gx10
		vLight[18] = textrc[74]; 	// Luz Gx1
		vLight[19] = textrc[77]; 	// Luz Bx100
		vLight[20] = textrc[78]; 	// Luz Bx10
		vLight[21] = textrc[79]; 	// Luz Bx1
		vLight[22] = textrc[82]; 	// Luz Garaje
		vLight[23] = textrc[85]; 	// Luz Jardín
		vLight[24] = textrc[88]; 	// Luz Porche
		vLight[25] = textrc[91]; 	// Luz Tendedero
		vLight[26] = textrc[94]; 	// Automático


		if(vLight[0] == '0' || vLiving[1] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, RESET); 			// Luz Sala
		else if(vLight[0] == '1' || vLiving[1] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, SET);

		if(vLight[1] == '0' || vLiving[2] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, RESET); 			// Luz Comedor
		else if(vLight[1] == '1'|| vLiving[2] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, SET);

		if(vLight[2] == '0' || vLiving[3] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, RESET); 			// Luz Ambiente
		else if(vLight[2] == '1' || vLiving[3] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, SET);

		if(vLight[3] == '0' || vLiving[4] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, RESET); 			// Luz Recibidor
		else if(vLight[3] == '1' || vLiving[4] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, SET);

		if(vLight[4] == '0' || vKitchen[0] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, RESET); 		// Luz Cocina
		else if(vLight[4] == '1' || vKitchen[0] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, SET);

		if(vLight[5] == '0' || vKitchen[1] == '0') HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET); 		// Luz Fregadero
		else if(vLight[5] == '1' || vKitchen[1] == '1')HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);

		if(vLight[6] == '0' || vBath[0] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, RESET); 			// Luz Baño
		else if(vLight[6] == '1' || vBath[0] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, SET);

		if(vLight[7] == '0' || vBath[1] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, RESET); 			// Luz Espejo
		else if(vLight[7] == '1' || vBath[1] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, SET);

		if(vLight[8] == '0' || vBedroom[0] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, RESET);			// Luz Dormitorio
		else if(vLight[8] == '1' || vBedroom[0] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, SET);

		if(vLight[9] == '0' || vBedroom[1] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET); 		// Luz Mesita Izq
		else if(vLight[9] == '1' || vBedroom[1] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);

		if(vLight[10] == '0' || vBedroom[2] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET); 		// Luz Mesita Dch
		else if(vLight[10] == '1' || vBedroom[2] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);

		if(vLight[11] == '0' || vOffice[0] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET); 		// Luz Oficina
		else if(vLight[11] == '1' || vOffice[0] == '1') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, SET);

		if(vLight[12] == '0' || vOffice[1] == '0'){ 													// Luz Gaming
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		}
		else if(vLight[12] == '1' || vOffice[1] == '1'){

			//uint8_t r, g, b;

			r = rgb_value (vLight[13],vLight[14],vLight[15]);
			g = rgb_value (vLight[16],vLight[17],vLight[18]);
			b = rgb_value (vLight[19],vLight[20],vLight[21]);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, g); // PA8
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, r); // PA9
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, b); // PA10
		}

		if(vLight[22] == '0' || vGarage[0] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, RESET); 		// Luz Garaje
		else if(vLight[22] == '1' || vGarage[0] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, SET);

		if(vLight[23] == '0' || vOutside[3] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET); 		// Luz Jardín
		else if(vLight[23] == '1' || vOutside[3] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);

		if(vLight[24] == '0' || vOutside[2] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET); 		// Luz Porche
		else if(vLight[24] == '1' || vOutside[2] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);

		if(vLight[25] == '0' || vOutside[1] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET); 		// Luz Tendedero
		else if(vLight[25] == '1' || vOutside[1] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);
	}

	// DOORS & WINDOWS
	if (fragment[0] == 'p'){
		UART_send("PUERTAS Y VENTANAS (PUERTA PARCELA)\n", PC_UART);
		vWindow[0] = textrc[28]; 	// P. Parcela
	}
	if (fragment[0] == 'g'){
		UART_send("PUERTAS Y VENTANAS (PUERTA GARAJE)\n", PC_UART);
		vWindow[1] = textrc[31]; 	// P. Garaje
	}
	/*if (fragment[0] == 'l'){
		UART_send("PUERTAS Y VENTANAS (VENTANA SALÓN)\n", PC_UART);
		vWindow[2] = textrc[34]; 	// V. Salón
	}*/
	if (fragment[0] == 'd'){
		UART_send("PUERTAS Y VENTANAS (VENTANA DORMITORIO)\n", PC_UART);
		vWindow[3] = textrc[37]; 	// V. Dormitorio
	}
	if (fragment[0] == 'o'){
		UART_send("PUERTAS Y VENTANAS (VENTANA OFICINA)\n", PC_UART);
		vWindow[4] = textrc[40]; 	// V. Oficina
	}

	// WEATHER
	if (fragment[0] == 't'){
		UART_send("TIEMPO\n", PC_UART);
		vWeather[0] = textrc[28]; 	// Ventilador
		//vWeather[1] = textrc[31]; 	// Vent. Dormitorio
		//vWeather[2] = textrc[34]; 	// Vent. Oficina
		vWeather[1] = textrc[31]; 	// Calefacción
		vWeather[2] = textrc[34]; 	// Automático


    }

	// OUTSIDE
	if (fragment[0] == 'e'){
		UART_send("LUCES (EXTERIOR)\n", PC_UART);
		//vOutside[0] = textrc[28]; 	// Toldo Tendedero
		vOutside[1] = textrc[31]; 	// Luz Tendedero
		vOutside[2] = textrc[34]; 	// Luz Porche
		vOutside[3] = textrc[37]; 	// Luz Jardín
		//vOutside[4] = textrc[40]; 	// Puerta Parcela
		vOutside[5] = textrc[43]; 	// Automático
		vOutside[6] = textrc[46]; 	// Ropa tendida

		if(vLight[25] == '0' || vOutside[1] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET); 		// Luz Tendedero
		else if(vLight[25] == '1' || vOutside[1] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);

		if(vLight[24] == '0' || vOutside[2] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET); 		// Luz Porche
		else if(vLight[24] == '1' || vOutside[2] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);

		if(vLight[23] == '0' || vOutside[3] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET); 		// Luz Jardín
		else if(vLight[23] == '1' || vOutside[3] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);
	}

	if (fragment[0] == 'P'){
		UART_send("PARCELA (EXTERIOR)\n", PC_UART);
		//vOutside[0] = textrc[28]; 	// Toldo Tendedero
		//vOutside[1] = textrc[31]; 	// Luz Tendedero
		//vOutside[2] = textrc[34]; 	// Luz Porche
		//vOutsidet[3] = textrc[37]; 	// Luz Jardín
		vOutside[4] = textrc[40]; 	// Puerta Parcela
	}

	if (fragment[0] == 'R'){
		UART_send("TENDEDERO (EXTERIOR)\n", PC_UART);
		vOutside[0] = textrc[28]; 	// Toldo Tendedero
		//vOutside[1] = textrc[31]; 	// Luz Tendedero
		//vOutside[2] = textrc[34]; 	// Luz Porche
		//vOutside[3] = textrc[37]; 	// Luz Jardín
		//vOutside[4] = textrc[40]; 	// Puerta Parcela
	}

	// LIVING ROOM
	if (fragment[0] == 'n'){
		UART_send("SALON \n", PC_UART);
		vLiving[0] = textrc[28]; 	// Televisión
		vLiving[1] = textrc[31]; 	// Luz Sala
		vLiving[2] = textrc[34]; 	// Luz Comedor
		vLiving[3] = textrc[37]; 	// Luz Ambiente
		vLiving[4] = textrc[40]; 	// Luz Recibidor


		if(vLiving[0] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, RESET); 							// Televisión
		else if(vLiving[0] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, SET);

		if(vLight[0] == '0' || vLiving[1] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, RESET); 		// Luz Sala
		else if(vLight[0] == '1' || vLiving[1] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_3, SET);

		if(vLight[1] == '0' || vLiving[2] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, RESET); 		// Luz Comedor
		else if(vLight[1] == '1' || vLiving[2] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, SET);

		if(vLight[2] == '0' || vLiving[3] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, RESET); 		// Luz Ambiente
		else if(vLight[2] == '1' || vLiving[3] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_5, SET);

		if(vLight[3] == '0' || vLiving[4] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, RESET); 		// Luz Recibidor
		else if(vLight[3] == '1' || vLiving[4] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, SET);
	}


	// KITCHEN
	if (fragment[0] == 'c'){
		UART_send("COCINA \n", PC_UART);
		vKitchen[0] = textrc[28]; 	// Luz Cocina
		vKitchen[1] = textrc[31]; 	// Luz Fregadero

		if(vLight[4] == '0' || vKitchen[0] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, RESET); 		// Luz Cocina
		else if(vLight[4] == '1' || vKitchen[0] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, SET);

		if(vLight[5] == '0' || vKitchen[1] == '0') HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, RESET); 		// Luz Fregadero
		else if(vLight[5] == '1' || vKitchen[1] == '1')HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, SET);
	}

	// BATHROOM
	if (fragment[0] == 'b'){
		UART_send("BAÑO \n", PC_UART);
		vBath[0] = textrc[28]; 	// Luz Baño
		vBath[1] = textrc[31]; 	// Luz Espejo

		if(vLight[6] == '0' || vBath[0] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, RESET); 		// Luz Baño
		else if(vLight[6] == '1' || vBath[0] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_6, SET);

		if(vLight[7] == '0' || vBath[1] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, RESET); 		// Luz Espejo
		else if(vLight[7] == '1' || vBath[1] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, SET);
	}

	// BEDROOM
	if (fragment[0] == 'r'){
		UART_send("LUCES (DORMITORIO) \n", PC_UART);
		vBedroom[0] = textrc[31]; 	// Luz Dormitorio
		vBedroom[1] = textrc[28]; 	// Luz Mesita Izq
		vBedroom[2] = textrc[34]; 	// Luz Mesita Dch
		//vBedroom[3] = textrc[37]; 	// V. Dormitorio

		if(vLight[8] == '0' || vBedroom[0] == '0') HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, RESET); 		// Luz Dormitorio
		else if(vLight[8] == '1' || vBedroom[0] == '1')HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, SET);

		if(vLight[9] == '0' || vBedroom[1] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, RESET); 		// Luz Mesita Izq
		else if(vLight[9] == '1' || vBedroom[1] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, SET);

		if(vLight[10] == '0' || vBedroom[2] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, RESET); 		// Luz Mesita Dch
		else if(vLight[10] == '1' || vBedroom[2] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, SET);
	}

	if (fragment[0] == 'D'){
		UART_send("V. DORMITORIO (DORMITORIO) \n", PC_UART);
		//vBedroom[0] = textrc[28]; 	// Luz Dormitorio
		//vBedroom[1] = textrc[31]; 	// Luz Mesita Izq
		//vBedroom[2] = textrc[34]; 	// Luz Mesita Dch
		vBedroom[3] = textrc[37]; 	// V. Dormitorio
	}

	// OFFICE
	if (fragment[0] == 'f'){
		UART_send("LUCES (OFICINA) \n", PC_UART);
		vOffice[0] = textrc[28]; 	// Luz Oficina
		vOffice[1] = textrc[31]; 	// Luz Gaming
		vOffice[2] = textrc[34]; 	// Luz Rx100
		vOffice[3] = textrc[35]; 	// Luz Rx10
		vOffice[4] = textrc[36];	// Luz Rx1
		vOffice[5] = textrc[39]; 	// Luz Gx100
		vOffice[6] = textrc[40]; 	// Luz Gx10
		vOffice[7] = textrc[41]; 	// Luz Gx1
		vOffice[8] = textrc[44];	// Luz Bx100
		vOffice[9] = textrc[45]; 	// Luz Bx10
		vOffice[10] = textrc[46];	// Luz Bx1
		//vOffice[11] = textrc[49]; 	// V. Oficina

		if(vLight[11] == '0' || vOffice[0] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, RESET); 		// Luz Oficina
		else if(vLight[11] == '1' || vOffice[0] == '1')HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, SET);

		if(vLight[12] == '0' || vOffice[1] == '0'){
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, 0);
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
		}
		else if(vLight[12] == '1' || vOffice[1] == '1'){ 												// Luz Gaming

			//uint8_t r, g, b;

			r = rgb_value(vOffice[2],vOffice[3],vOffice[4]);
			g = rgb_value(vOffice[5],vOffice[6],vOffice[7]);
			b = rgb_value(vOffice[8],vOffice[9],vOffice[10]);

			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, g); // PA8
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, r); // PA9
			__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, b); // PA10
		}
	}

	if (fragment[0] == 'F'){
		UART_send("V. OFICINA (OFICINA) \n", PC_UART);
		//vOffice[0] = textrc[28]; 	// Luz Oficina
		//vOffice[1] = textrc[31]; 	// Luz Gaming
		//vOffice[2] = textrc[34]; 	// Luz Rx100
		//vOffice[3] = textrc[35]; 	// Luz Rx10
		//vOffice[4] = textrc[36]; 	// Luz Rx1
		//vOffice[5] = textrc[39]; 	// Luz Gx100
		//vOffice[6] = textrc[40]; 	// Luz Gx10
		//vOffice[7] = textrc[41]; 	// Luz Gx1
		//vOffice[8] = textrc[44]; 	// Luz Bx100
		//vOffice[9] = textrc[45]; 	// Luz Bx10
		//vOffice[10] = textrc[46]; 	// Luz Bx1
		vOffice[11] = textrc[49]; 	// V. Oficina
	}

	// GARAGE
	if (fragment[0] == 'j'){
		UART_send("LUZ GARAJE (GARAJE) \n", PC_UART);
		vGarage[0] = textrc[28]; 	// Luz Garaje
		//vGarage[1] = textrc[31]; 	// P. Garaje

		if(vLight[22] == '0' || vGarage[0] == '0') HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, RESET);		// Luz Garaje
		else if(vLight[22] == '1' || vGarage[0] == '1')HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, SET);
	}

	if (fragment[0] == 'J'){
		UART_send("P. GARAJE (GARAJE) \n", PC_UART);
		//vGarage[0] = textrc[28]; 	// Luz Garaje
		vGarage[1] = textrc[31]; 	// P. Garaje
	}

	// GARDEN
	if (fragment[0] == 'h'){
		UART_send("RIEGO HUERTO \n", PC_UART);
		vGarden[0] = textrc[28]; 	// Riego
		vGarden[1] = textrc[31]; 	// Automático

		/*if(vHuer[1] == '0'){
			if(vHuer[0] == '0') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, RESET); 							// Riego
			else if(vHuer[0] == '1') HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, SET);
		}*/
	}

	// SETTINGS
	if (fragment[0] == 'a'){
		UART_send("AJUSTES \n", PC_UART);
		vSettings[0] = textrc[28]; 		// Vent. Enc. x 10
		vSettings[1] = textrc[29]; 		// Vent. Enc. x 1
		vSettings[2] = textrc[31]; 		// Vent. Enc. x 0.1
		vSettings[3] = textrc[34]; 		// Vent. Apa. x 10
		vSettings[4] = textrc[35]; 		// Vent. Apa. x 1
		vSettings[5] = textrc[37]; 		// Vent. Apa. x 0.1
		vSettings[6] = textrc[40]; 		// Calef. Enc. x 10
		vSettings[7] = textrc[41]; 		// Calef. Enc. x 1
		vSettings[8] = textrc[43]; 		// Calef. Enc. x 0.1
		vSettings[9] = textrc[46]; 		// Calef. Apa. x 10
		vSettings[10] = textrc[47]; 	// Calef. Apa. x 1
		vSettings[11] = textrc[49]; 	// Calef. Apa. x 0.1
		vSettings[12] = textrc[52]; 	// RH Mínimo x 10
		vSettings[13] = textrc[53]; 	// RH Mínimo x 1
		vSettings[14] = textrc[56]; 	// RH Máximo x 10
		vSettings[15] = textrc[57]; 	// RH Máximo x 1
	}

	//__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}
