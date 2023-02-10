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
#include "Buzzer.h"
#include "ESP8266_HAL.h"
#include "LED.h"
#include "RH.h"
#include "SG90.h"
#include "DHT22.h"
#include "Temperature.h"
#include "gfx.h"
#include "fonts.h"
#include "ssd1306.h"

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

I2C_HandleTypeDef hi2c1;

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

volatile int doorbell = 0;
volatile int stop = 0;
volatile int inside = 0;
volatile int outside = 0;

volatile float f_on = 0; 		// Fan ON
volatile float f_off = 0; 		// Fan OFF
volatile float h_on = 0; 		// Heat ON
volatile float h_off = 0; 		// Heat OFF

volatile int rh_min = 0;
volatile int rh_max = 0;

volatile int awning = 0; 		// 1=unfolded 	0=folded
volatile int parcel = 0; 		// 1=opened 	0=closed
volatile int garage = 0; 		// 1=opened 	0=closed
volatile int office = 0; 		// 1=up 		0=down
volatile int bedroom = 0; 		// 1=up 		0=down

volatile int water = 0; 		// 1=on 		0=off
volatile int fan = 0; 			// 1=on 		0=off
volatile int heat = 0; 			// 1=on 		0=off


/*----------- Sensors -----------*/

// Brightness Sensor (LDR)
uint16_t LDR_value = 0;

// Hygrometer (HW-390)
uint16_t Hygro_read = 0;
uint16_t Hygro = 0;

// Rain Sensor
uint16_t Rain_read = 0;
uint16_t Rain = 0;

// Humidity and Temperature Sensor (outside, units, DHT22)
DHT22_DataTypedef DHT22_outside;
int TempOutside = 0;
int RHOutside = 0;

// Humidity and Temperature Sensor (inside, decimals, DHT22)
DHT22_DataTypedef DHT22_inside;
float TempInside = 0;
float RHInside = 0;

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
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    if (GPIO_Pin==S_In_Pin)
    {
        inside = 1;
    }
    if (GPIO_Pin==S_Out_Pin)
    {
        outside = 1;
    }
}

int debouncer(volatile int* button_int, GPIO_TypeDef* GPIO_port, uint16_t GPIO_number){ // Bounce Control

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
			if (button_count==4 ) // Debounce Period
			{
				button_count=0;
				*button_int=0;
				return 1;
			}
		}
	}
	return 0;
}


/*----------- Delay (ms) -----------*/
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  // RGB Gaming LED
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

  // Parcel Servo
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  // Garage Servo
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);

  // Swning Servo
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);

  // Living Room Servo
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);

  // Bedroom Servo
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);

  // Office Servo
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

  // Living Room DC Motor
  HAL_TIM_PWM_Start(&htim9, TIM_CHANNEL_1);

  // Buzzer
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);

  /*----------- DHT22 Sensor-----------*/
  HAL_TIM_Base_Start(&htim6);

  /*----------- LDR & HW390 & Rain Initialization  -----------*/
  HAL_ADC_Start(&hadc1); // LDR
  HAL_ADC_Start(&hadc2); // HW-390
  HAL_ADC_Start(&hadc3); // Rain

  //__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
  ESP_Init("iPhone Carmela","pistacho");

  // OLED Thermostat
  oled_init(); // Initialize OLED

  // Frame
  line_h(0, 127, 1, 2, 1);
  line_h(0, 127, 30, 2, 1);
  line_v(0, 31, 1, 2, 1);
  line_v(0, 31, 127, 2, 1);

  // Line
  line_h(5, 122, 15, 1, 1);

  graphics_text(7, 5, FONT_SEVEN_DOT, "IN");
  graphics_text(7, 19, FONT_SEVEN_DOT, "OUT");

  oled_update(); // Update OLED

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1){
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  	ESP_messageHandler();

		// Vehicle Doorbell
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_0) == 0){

			int known = readRFID();
			if(known == 1){
				char kn[15] = "OPENING DOOR \n\n";
				HAL_UART_Transmit(&huart6, (uint8_t *) kn, 15, HAL_MAX_DELAY);

				actParcelRFID();
			}
			else if(known == 0){
				char unkn[19] = "CALLING THE BELL \n\n";
				HAL_UART_Transmit(&huart6, (uint8_t *) unkn, 19, HAL_MAX_DELAY);

				playBell();
			}
		}

		// People Doorbell
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == 0){

				playBell();
		}

		// Alarm Stop
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_15) == 0){

			__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, 0);
		}

		// Alarm
		if (debouncer(&inside, S_In_GPIO_Port, S_In_Pin)){

			if(vSecurity[0] == '1') playAlarm();
		}
		if (debouncer(&outside, S_Out_GPIO_Port, S_Out_Pin)){

			if(vSecurity[1] == '1') playAlarm();
		}

		// PARCEL DOOR (90)
		if(vWindow[0]=='1' || vOutside[4]=='1') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 91);
		if(vWindow[0]=='0' || vOutside[4]=='0') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89);

		// GARAGE DOOR (90)
		if(vWindow[1]=='1' || vGarage[1] == '1') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 92); // Faster at 30
		if(vWindow[1]=='0' || vGarage[1] == '0') __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 89);

		// CLOTHES LINE AWNING (90)
		if(vOutside[0]=='1'){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 91);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
			awning = 1;
		}
		if(vOutside[0]=='0'){
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 88);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
			awning = 0;
		}
		vOutside[0]='x';

		// LIVING ROOM WINDOW (90)
		if(vWindow[2]=='1' || vLiving[5]=='1') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 91);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 90);
		}
		if(vWindow[2]=='0'|| vLiving[5]=='0') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 89);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_4, 90);
		}
		vWindow[2]='x';
		vLiving[5]='x';

		// BEDROOM WINDOW (90)
		if(vWindow[3]=='1' || vBedroom[3]=='1') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 92);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 90);
		}
		if(vWindow[3]=='0' || vBedroom[3]=='0') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 89);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 90);
		}
		vWindow[3]='x';
		vBedroom[3]='x';

		// OFFICE WINDOW (90)
		if(vWindow[4]=='1' || vOffice[11]=='1') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 91);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 90);
		}
		if(vWindow[4]=='0' || vOffice[11]=='0') {
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 88);
			HAL_Delay(3000);
			__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 90);
		}
		vWindow[4]='x';
		vOffice[11]='x';

		// PARCEL LIMIT SWITCH
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6) == 0){

			if (vWindow[0]=='1' || vOutside[4]=='1'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 89); // Parcel Servo
				HAL_Delay(1000);
			}
			if (vWindow[0]=='0' || vOutside[4]=='0'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 91); // Parcel Servo
				HAL_Delay(1000);
			}
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 90); // Parcel Servo
			vWindow[0]='x'; // Parcel Servo
			vOutside[4]='x'; // Parcel Servo
		}

		// GARAGE LIMIT SWITCH
		if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4) == 0){

			if (vWindow[1]=='1' || vGarage[1]=='1'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 89); // Garage Servo
				HAL_Delay(1000);
			}
			if (vWindow[1]=='0' || vGarage[1]=='0'){
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 92); // Garage Servo
				HAL_Delay(1000);
			}
			__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 90); // Garage Servo
			vWindow[1]='x'; // Garage Servo
			vGarage[1]='x'; // Garage Servo
		}

		// LINVING ROOM FAN
		if(vWeather[0]=='1') {
			__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 1000);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_RESET);
		}
		if(vWeather[0]=='0') {
			__HAL_TIM_SET_COMPARE(&htim9, TIM_CHANNEL_1, 0);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,GPIO_PIN_RESET);
		}

		// SETTINGS
		f_on = temp_value(vSettings[0], vSettings[1], vSettings[2]);
		f_off = temp_value(vSettings[3], vSettings[4], vSettings[5]);
		h_on = temp_value(vSettings[6], vSettings[7], vSettings[8]);
		h_off = temp_value(vSettings[9], vSettings[10], vSettings[11]);

		rh_min = rh_value(vSettings[12], vSettings[13]);
		rh_max = rh_value(vSettings[14], vSettings[15]);


		/*----------- Sensor Reading -----------*/

		// LDR
		if(HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY) == HAL_OK)
			LDR_value = HAL_ADC_GetValue(&hadc1);

		ldr(LDR_value);

		// Rain
		if(HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY) == HAL_OK)
			Rain_read = HAL_ADC_GetValue(&hadc3);

		Rain = 100 - ((100*Rain_read)/255);

		if(vOutside[5] == '1'){
			// There are clothes and it is raining (closed awning)
			if (Rain>5 && vOutside[6] == '1' && awning == 0){

				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 91); // Open awning
				HAL_Delay(3000);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
				awning = 1;
			}

			// There are clothes and it is raining (opened awning)
			// There are not clothes and it is raining (closed awning)

			// There are not clothes and it is raining (opened awning)
			else if (Rain>5 && vOutside[6] == '0' && awning == 1){

				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 88); // Close awning
				HAL_Delay(3000);
				__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, 90);
				awning = 0;
			}

			// There are clothes and it is not raining (awning does not matter)
			// There are not clothes and it is not raining (awning does not matter)

			vOutside[0]='x';
		}

		// HW-390
		if(HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY) == HAL_OK)
			Hygro_read = HAL_ADC_GetValue(&hadc2);

		Hygro = 100 - ((100*Hygro_read)/255);

		/*if(vGarden[1] == '1'){
			// It is turned off and it do not reach the minimum RH or it is turned on and it do not reach the maximum RH
			if (((!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) && (Hygro<rh_min)) || ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) && (Hygro<rh_max))){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, SET); // Water
			}
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, RESET); // Water
		}*/

		//DHT22
		if(readDHT != 0){

		  	// Inside
		  	DHT22_getData(&DHT22_inside);
		  	RHInside = DHT22_inside.Humidity;
		  	TempInside = DHT22_inside.Temperature;

		    int in_rh_unit = RHInside;
		    int in_rh_dec = (RHInside-in_rh_unit)*10;
		    char c_in_rh_unit[2];
		    char c_in_rh_dec[1];
		    itoa(in_rh_unit, c_in_rh_unit, 10);
		    itoa(in_rh_dec, c_in_rh_dec, 10);

		    int in_temp_unit = TempInside;
		    int in_temp_dec = (TempInside-in_temp_unit)*10;
		    char c_in_temp_unit[2];
		    char c_in_temp_dec[1];
		    itoa(in_temp_unit, c_in_temp_unit, 10);
		    itoa(in_temp_dec, c_in_temp_dec, 10);

		    clear();

		    // Frame
		    line_h(0, 127, 1, 2, 1);
		    line_h(0, 127, 30, 2, 1);
		    line_v(0, 31, 1, 2, 1);
		    line_v(0, 31, 127, 2, 1);

		    // Line
		    line_h(5, 122, 15, 1, 1);

		    graphics_text(7, 5, FONT_SEVEN_DOT, "IN");
		    graphics_text(7, 19, FONT_SEVEN_DOT, "OUT");

		    // Data
		    graphics_text(44, 6, FONT_SIX_DOT, "RH      .");
		    graphics_text(56, 6, FONT_SIX_DOT, c_in_rh_unit);
		    graphics_text(68, 6, FONT_SIX_DOT, c_in_rh_dec);
		    graphics_text(75, 6, FONT_SIX_DOT, "%");

		    graphics_text(90, 6, FONT_SIX_DOT, "T      .");
		    graphics_text(98, 6, FONT_SIX_DOT, c_in_temp_unit);
		    graphics_text(110, 6, FONT_SIX_DOT, c_in_temp_dec);
		    graphics_text(117, 6, FONT_SIX_DOT, "C");

			// Exterior
			DHT22_getData(&DHT22_outside);
		  	RHOutside = DHT22_outside.Humidity;
		  	TempOutside = DHT22_outside.Temperature;

		    int out_rh_unit = RHOutside;
		    char c_out_rh_unit[2];
		    itoa(out_rh_unit, c_out_rh_unit, 10);

		    int out_temp_unit = TempOutside;
		    char c_out_temp_unit[2];
		    itoa(out_temp_unit, c_out_temp_unit, 10);

		    graphics_text(44, 20, FONT_SIX_DOT, "RH      .0 %");
		    graphics_text(56, 20, FONT_SIX_DOT, c_out_rh_unit);

		    graphics_text(90, 20, FONT_SIX_DOT, "T      .0 C");
		    graphics_text(98, 20, FONT_SIX_DOT, c_out_temp_unit);


		    oled_update(); // Update OLED

		  	readDHT = 0;
		}

		if(vWeather[4] == '1'){
			// Fan is turned off and it reach the maximum temperature or it is turned off and it does not reach the minimum temperature
			/*if (((!HAL_GPIO_ReadPin(GPIOX, GPIO_PIN_X)) && (TempInside>f_on)) || ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) && (TempInside>f_off))){
				HAL_GPIO_WritePin(GPIOX, GPIO_PIN_XX, SET); // Living Room Fan
			}
			else HAL_GPIO_WritePin(GPIOX, GPIO_PIN_X, RESET); // Living Room Fan*/

			// Heat is turned off and it does not reach the minimum temperature or it is turned on and it does not reach the maximum temperature
			if (((!HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15)) && (TempInside<h_on)) || ((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)) && (TempInside<h_off))){
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, SET); // Heat
			}
			else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, RESET); // Heat
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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  TIM_OC_InitTypeDef sConfigOC = {0};

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
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

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
  huart3.Init.BaudRate = 9600;
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
  huart6.Init.BaudRate = 115200;
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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, WiFi_OK_Pin|Living_Fan_1_Pin|Livin_Fan_2_Pin|L_Kitchen_Pin
                          |L_Garage_Pin|L_ClothesLine_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, DHT22_In_Pin|DHT22_Out_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Water_Pin|Heat_Pin|L_Mirror_Pin|L_Left_Pin
                          |L_Office_Pin|L_Right_Pin|L_TV_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(L_Sink_GPIO_Port, L_Sink_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, L_Hall_Pin|L_Dining_Pin|L_Garden_Pin|L_Living_Pin
                          |L_Porch_Pin|L_Relax_Pin|L_Bathroom_Pin|L_Bedroom_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : S_In_Pin Parcel_Limit_Pin S_Out_Pin */
  GPIO_InitStruct.Pin = S_In_Pin|Parcel_Limit_Pin|S_Out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : Parcel_Garage_Pin */
  GPIO_InitStruct.Pin = Parcel_Garage_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Parcel_Garage_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : B_People_Bell_Pin B_Stop_Pin */
  GPIO_InitStruct.Pin = B_People_Bell_Pin|B_Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : WiFi_OK_Pin Living_Fan_1_Pin Livin_Fan_2_Pin L_Kitchen_Pin
                           L_Garage_Pin L_ClothesLine_Pin */
  GPIO_InitStruct.Pin = WiFi_OK_Pin|Living_Fan_1_Pin|Livin_Fan_2_Pin|L_Kitchen_Pin
                          |L_Garage_Pin|L_ClothesLine_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : DHT22_In_Pin DHT22_Out_Pin */
  GPIO_InitStruct.Pin = DHT22_In_Pin|DHT22_Out_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : Water_Pin Heat_Pin L_Mirror_Pin L_Left_Pin
                           L_Office_Pin L_Right_Pin L_TV_Pin */
  GPIO_InitStruct.Pin = Water_Pin|Heat_Pin|L_Mirror_Pin|L_Left_Pin
                          |L_Office_Pin|L_Right_Pin|L_TV_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : L_Sink_Pin */
  GPIO_InitStruct.Pin = L_Sink_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(L_Sink_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : L_Hall_Pin L_Dining_Pin L_Garden_Pin L_Living_Pin
                           L_Porch_Pin L_Relax_Pin L_Bathroom_Pin L_Bedroom_Pin */
  GPIO_InitStruct.Pin = L_Hall_Pin|L_Dining_Pin|L_Garden_Pin|L_Living_Pin
                          |L_Porch_Pin|L_Relax_Pin|L_Bathroom_Pin|L_Bedroom_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : B_Vehicle_Bell_Pin */
  GPIO_InitStruct.Pin = B_Vehicle_Bell_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(B_Vehicle_Bell_GPIO_Port, &GPIO_InitStruct);

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

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

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
