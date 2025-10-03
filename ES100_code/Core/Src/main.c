/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "button.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct ESCconfig{
	uint8_t firstDigit;
	uint8_t secondDigit;
	uint8_t fastAcceleration;
	uint8_t kmph;
	uint8_t fifthDigit;
	uint8_t light;
	uint8_t lightBlink;
	uint8_t enable;
	uint8_t speedLimit;
}ESCconfig_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define BUTTON_CHECK(GPIOx,GPIO_Pin) HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t lastTick =0;//for non-blocking intervals

ESCconfig_t ESC ={1,0,0,1,0,0,0,1,228};//esc initial configuration

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/**
  * @brief  CRC8 (Maxim/Dallas) calculation
  * @retval uint8_t
  */
uint8_t CRC8_maxim(uint8_t *data);

void Button_ShortRelease(uint8_t btnCode);
void Button_LongPress(uint8_t btnCode);
void Button_LongRelease(uint8_t btnCode);

void InitialLongPress(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_USART1_UART_Init();
  MX_TIM17_Init();
  /* USER CODE BEGIN 2 */


  btn_instance_t keys[1] = {
      {
          .port = BUTTON_GPIO_Port,
          .pin = BUTTON_Pin,
      },
  };

  btn_init_t btn_init = {
      .process_time_ms = 10,
      .debounce_time_ms = 20,
      .port_read = (port_read_cb_t)&HAL_GPIO_ReadPin,
      .short_release = Button_ShortRelease,
      .long_release = Button_LongRelease,
      .long_press = Button_LongPress,
  };

  InitialLongPress(BUTTON_GPIO_Port, BUTTON_Pin);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  BUTTON_CHECK(GPIOA, GPIO_PIN_2);
	  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
	  HAL_Delay(200);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV2;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if (htim->Instance == TIM17){
		Button_Update();
	}

}

void InitialLongPress(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin){
	while(1){
		while(HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==1){//do until press event
				HAL_Delay(1);
			}
			HAL_Delay(19);
			//debounce check
			if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==1){
				return; //there was a noise
			}
			//duration measurement
			uint32_t pressDuration = HAL_GetTick();
			while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==0){
				if (HAL_GetTick()- pressDuration >= 1500){
					HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
					HAL_GPIO_WritePin(ON_GPIO_Port, ON_Pin, GPIO_PIN_SET);

					while (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==0){//do until release event
						HAL_Delay(1);
					}
					HAL_Delay(20);
					if (HAL_GPIO_ReadPin(GPIOx, GPIO_Pin)==1){// if the button is released
						//do something
						HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
					}
					return;
				}
				HAL_Delay(1);
			}
	}


}

void Button_ShortRelease(uint8_t btnCode){

}
void Button_LongPress(uint8_t btnCode){

}
void Button_LongRelease(uint8_t btnCode){

}


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
