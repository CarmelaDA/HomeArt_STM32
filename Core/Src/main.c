/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *                             
  *---------------------------------- HOMEART ----------------------------------
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ESP8266_HAL.h"
#include "LDR.h"
#include "DHT11.h"
#include "DHT22.h"
#include "RH.h"
#include "RFID.h"
#include "SG90.h"
#include "Temperatura.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WiFi_UART &huart2
#define PC_UART &huart6
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
ADC_HandleTypeDef hadc3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim9;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

/* USER CODE BEGIN PV */

volatile int timbre = 0;
volatile int stop = 0;
volatile int interior = 0;
volatile int exterior = 0;

volatile float v_enc = 0;
volatile float v_apa = 0;
volatile float c_enc = 0;
volatile float c_apa = 0;

volatile int rh_min = 0;
volatile int rh_max = 0;

volatile int toldo = 0; // 1=abierto 0=cerrado


/*----------- Sensores -----------*/

// Sensor de Luminosidad (LDR)
uint16_t LDR_valor = 0;

// Higrómetro (HW-390)
uint16_t Higro_lectura = 0;
uint16_t Higro_real = 0;

// Sensor de Lluvia
uint16_t Lluvia_lectura = 0;
uint16_t Lluvia_real = 0;

// Sensor de Temperatura y Humedad del Aire (exterior, DHT11)
DHT11_DataTypedef DHT11;
volatile int TempAireExt = 0;
volatile int HumeAireExt = 0;

// Sensor de Temperatura y Humedad del Aire (interior, DHT22)
DHT22_DataTypedef DHT22;
float TempAireInt = 0;
float HumeAireInt = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM8_Init(void);
static void MX_TIM9_Init(void);
static void MX_TIM6_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
static void MX_ADC3_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
    if (GPIO_Pin==B_Timbre_Pin)
	{
		timbre = 1;
	}
    if (GPIO_Pin==B_Stop_Pin)
    {
    	stop = 1;
    }
    if (GPIO_Pin==S_Int_Pin)
    {
        interior = 1;
    }
    if (GPIO_Pin==S_Ext_Pin)
    {
        exterior = 1;
    }
}

int debouncer(volatile int* button_int, GPIO_TypeDef* GPIO_port, uint16_t GPIO_number) // Control de los rebotes
{
	static uint8_t button_count=0;
	static int counter=0;

	if (*button_int==1)
	{
		if (button_count==0)
		{
			counter=HAL_GetTick();
			button_count++;
		}
		if (HAL_GetTick()-counter>=20)
		{
			counter=HAL_GetTick();
			if (HAL_GPIO_ReadPin(GPIO_port, GPIO_number)!=1)
			{
				button_count=1;
			}
			else
			{
				button_count++;
			}
			if (button_count==4 ) // Periodo antirebotes
			{
				button_count=0;
				*button_int=0;
				return 1;
			}
		}
	}
	return 0;
}

void playTimbre(){

	uint8_t tone;

	tone = 25;
	__HAL_TIM_SET_AUTORELOAD(&htim4, tone*2);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tone);
	HAL_Delay(300);

	tone = 40;
	__HAL_TIM_SET_AUTORELOAD(&htim4, tone*2);
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tone);
	HAL_Delay(800);

	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
}


void playAlarma(){

	uint8_t tone;

	for(tone = 40; tone >= 10; tone = tone-10){
		__HAL_TIM_SET_AUTORELOAD(&htim4, tone*2);
		__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, tone);
		HAL_Delay(300);
	}
}

void luces_automat(int i){

	if(i){
		//vIlum[23]='1';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, SET);
		//vIlum[24]='1';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, SET);
		//vIlum[25]='1';
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, SET);
	}
	else{
		//vIlum[23]='0';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, RESET);
		//vIlum[24]='0';
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, RESET);
		//vIlum[25]='0';
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, RESET);
	}
}


/*----------- Delay en Microsegundos -----------*/
void delay(uint16_t time){

	__HAL_TIM_SET_COUNTER(&htim6,0);
	while((__HAL_TIM_GET_COUNTER(&htim6))<time);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM5_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM8_Init();
  MX_TIM9_Init();
  MX_TIM6_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_ADC3_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  // LED RGB Gaming
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
  // Servo Parcela
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  // Servo Garaje
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  // Servo Tendedero
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
  // Servo Salón
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
  // Servo Dormitorio
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
  // Servo Oficina
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  // DC Salón
  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);
  // Zumbador
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

  /*----------- Útil para Sensor DHT11/22 -----------*/
  HAL_TIM_Base_Start(&htim6);

  /*----------- Inicialización LDR & HW390 & Lluvia -----------*/
  HAL_ADC_Start(&hadc1); // LDR
  HAL_ADC_Start(&hadc2); // HW-390
  HAL_ADC_Start(&hadc3); // Lluvia

  //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  ESP_Init("iPhone Carmela","pistacho");
  //ESP_Init("iPhone de Mario","11111111");
  //ESP_Init("MPIGDATA","B88458609A");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	ESP_messageHandler();

		// TIMBRE
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0) == 0){

			if(readLector()){
				//actParcelaRFID();
			}
			else playTimbre();
		}

		// STOP ALARMA
		// Conexión Wi-Fi

		// ALARMA
		if (debouncer(&interior, S_Int_GPIO_Port, S_Int_Pin)){
			if(vSeg[0] == '1') playAlarma();
		}
		if (debouncer(&exterior, S_Ext_GPIO_Port, S_Ext_Pin)){
			if(vSeg[1] == '1') playAlarma();
		}

		// PUERTA PARCELA (90)
		if(vVent[0]=='1' || vExt[4]=='1') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 92);
		if(vVent[0]=='0' || vExt[4]=='0') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89);

		// PUERTA GARAJE (90)
		if(vVent[1]=='1' || vGar[1] == '1') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 91); // más rápido a 30
		if(vVent[1]=='0' || vGar[1] == '0') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 89);

		// TOLDO TENDEDERO (90)
		if(vExt[0]=='1'){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 91);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
			toldo = 1;
		}
		if(vExt[0]=='0'){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 88);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
			toldo = 0;
		}
		vExt[0]='x';

		// VENTANA SALÓN (90)
		if(vVent[2]=='1' || vSal[5]=='1') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 91);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 90);
		}
		if(vVent[2]=='0'|| vSal[5]=='0') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 89);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 90);
		}
		vVent[2]='x';
		vSal[5]='x';

		// VENTANA DORMITORIO (90)
		if(vVent[3]=='1' || vDor[3]=='1') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 92);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 90);
		}
		if(vVent[3]=='0' || vDor[3]=='0') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 89);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 90);
		}
		vVent[3]='x';
		vDor[3]='x';

		// VENTANA OFICINA (90)
		if(vVent[4]=='1' || vOfi[11]=='1') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 91);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 90);
		}
		if(vVent[4]=='0' || vOfi[11]=='0') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 88);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 90);
		}
		vVent[4]='x';
		vOfi[11]='x';

		// FINAL DE CARRERA PARCELA
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == 0){

			if (vVent[0]=='1' || vExt[4]=='1'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89); // S_Parcela
				HAL_Delay(1000);
			}
			if (vVent[0]=='0' || vExt[4]=='0'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 92); // S_Parcela
				HAL_Delay(1000);
			}
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // S_Parcela
			vVent[0]='x'; // S_Parcela
			vExt[4]='x'; // S_Parcela
		}

		// FINAL DE CARRERA GARAJE
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == 0){

			if (vVent[1]=='1' || vGar[1]=='1'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 89); // S_Garaje
				HAL_Delay(1000);
			}
			if (vVent[1]=='0' || vGar[1]=='0'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 91); // S_Garaje
				HAL_Delay(1000);
			}
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 90); // S_Garaje
			vVent[1]='x'; // S_Garaje
			vGar[1]='x'; // S_Garaje
		}

		// VENTILADOR SALÓN
		if(vTemp[0]=='1') {
			__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 1000);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_RESET);
		}
		if(vTemp[0]=='0') {
			__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_RESET);
		}

		// VALORES DE CONFIGURACIÓN

		v_enc = temp_value(vAj[0], vAj[1], vAj[2]);
		v_apa = temp_value(vAj[3], vAj[4], vAj[5]);
		c_enc = temp_value(vAj[6], vAj[7], vAj[8]);
		c_apa = temp_value(vAj[9], vAj[10], vAj[11]);

		rh_min = rh_value(vAj[12], vAj[13]);
		rh_max = rh_value(vAj[14], vAj[15]);

		/*----------- Lectura Sensores -----------*/

		// LDR
		if(HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
			LDR_valor = HAL_ADC_GetValue(&hadc1);

		ldr(LDR_valor);

		// Lluvia
		if(HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY) == HAL_OK)
			Lluvia_lectura = HAL_ADC_GetValue(&hadc3);

		Lluvia_real = 100 - ((100*Lluvia_lectura)/255);

		if(vExt[5] == '1'){
			// Si hay ropa tendida y llueve (con tendedero cerrado)
			if (Lluvia_real>5 && vExt[6] == '1' && toldo == 0){

				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 91); // Abrir tendedero
				HAL_Delay(3000);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
				toldo = 1;
			}

			// Si hay ropa tendida y llueve (con tendedero abierto)
			// Si no hay ropa tendida y llueve (con tendedero cerrado)

			// Si no hay ropa tendida y llueve (con tendedero abierto)
			else if (Lluvia_real>5 && vExt[6] == '0' && toldo == 1){

				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 88); // Cerrar tendedero
				HAL_Delay(3000);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
				toldo = 0;
			}

			// Si hay ropa tendida y no llueve (da igual el tendedero)
			// Si hay no hay ropa tendida y no llueve (da igual el tendedero)

			vExt[0]='x';
		}

		// HW-390
		if(HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK)
			Higro_lectura = HAL_ADC_GetValue(&hadc2);

		Higro_real = 100 - ((100*Higro_lectura)/255);

		/*if(vHuer[1] == '1'){
			// Si está apagado y no llega al mínimo o está encendido y no llega al máximo
			if (((!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) && (Higro_real<rh_min)) || ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) && (Higro_real<rh_max))){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, SET); // Riego
			}
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, RESET); // Riego
		}*/

		//DHT22
		if(actSensor == 1){
			// DHT11
			/*DHT11_getData(&DHT11);
		  	TempAireExt = DHT11.Temperature;
		  	HumeAireExt = DHT11.Humidity;*/

		  	// DHT22 Interior
		  	DHT22_getData(&DHT22);
		  	TempAireInt = DHT22.Temperature;
		  	HumeAireInt = DHT22.Humidity;
		}

		if(vTemp[4] == '1'){
			// Si está apagado y no llega al mínimo o está encendido y no llega al máximo
			if (((!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) && (TempAireInt<c_enc)) || ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) && (TempAireInt<c_apa))){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, SET); // Calefacción
			}
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, RESET); // Calefacción
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_8B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc2.Init.Resolution = ADC_RESOLUTION_8B;
  hadc2.Init.ScanConvMode = DISABLE;
  hadc2.Init.ContinuousConvMode = ENABLE;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_8;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc3.Init.Resolution = ADC_RESOLUTION_8B;
  hadc3.Init.ScanConvMode = DISABLE;
  hadc3.Init.ContinuousConvMode = ENABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 1;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 96-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 255-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 500-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 2000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 500-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 2000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 96-1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 20;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 96-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 2000-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 50-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0xffff-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim8, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 500-1;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 2000-1;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim9, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */
  HAL_TIM_MspPostInit(&htim9);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 9600;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, WiFi_OK_Pin|DC_Salon_1_Pin|DC_Salon_2_Pin|L_Cocina_Pin
                          |L_Garaje_Pin|L_Tendedero_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, DHT22_Pin|DHT11_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Riego_Pin|Peltier_Pin|L_Espejo_Pin|L_Izquierda_Pin
                          |L_Oficina_Pin|L_Derecha_Pin|L_TV_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(L_Fregadero_GPIO_Port, L_Fregadero_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, L_Recibidor_Pin|L_Comedor_Pin|L_Jardin_Pin|L_Sala_Pin
                          |L_Porche_Pin|L_Ambiente_Pin|L_Bano_Pin|L_Dormitorio_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : B_Stop_Pin Fin_Garaje_Pin */
  GPIO_InitStruct.Pin = B_Stop_Pin|Fin_Garaje_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : S_Int_Pin Fin_Parcela_Pin S_Ext_Pin */
  GPIO_InitStruct.Pin = S_Int_Pin|Fin_Parcela_Pin|S_Ext_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : WiFi_OK_Pin DC_Salon_1_Pin DC_Salon_2_Pin L_Cocina_Pin
                           L_Garaje_Pin L_Tendedero_Pin */
  GPIO_InitStruct.Pin = WiFi_OK_Pin|DC_Salon_1_Pin|DC_Salon_2_Pin|L_Cocina_Pin
                          |L_Garaje_Pin|L_Tendedero_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DHT22_Pin DHT11_Pin */
  GPIO_InitStruct.Pin = DHT22_Pin|DHT11_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : Riego_Pin Peltier_Pin L_Espejo_Pin L_Izquierda_Pin
                           L_Oficina_Pin L_Derecha_Pin L_TV_Pin */
  GPIO_InitStruct.Pin = Riego_Pin|Peltier_Pin|L_Espejo_Pin|L_Izquierda_Pin
                          |L_Oficina_Pin|L_Derecha_Pin|L_TV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : L_Fregadero_Pin */
  GPIO_InitStruct.Pin = L_Fregadero_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(L_Fregadero_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : L_Recibidor_Pin L_Comedor_Pin L_Jardin_Pin L_Sala_Pin
                           L_Porche_Pin L_Ambiente_Pin L_Bano_Pin L_Dormitorio_Pin */
  GPIO_InitStruct.Pin = L_Recibidor_Pin|L_Comedor_Pin|L_Jardin_Pin|L_Sala_Pin
                          |L_Porche_Pin|L_Ambiente_Pin|L_Bano_Pin|L_Dormitorio_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : B_Timbre_Pin */
  GPIO_InitStruct.Pin = B_Timbre_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(B_Timbre_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
