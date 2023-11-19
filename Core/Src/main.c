/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "spi.h"
#include "tim.h"
#include "font.h"
#include "gpio.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern volatile uint16_t flag;

extern uint8_t data1[16], data2[16], data3[16], data4[16];
char *pMyStr = "You";  //Строка на вывод
char *pMyStr2 = "Lose";

uint16_t xs = 3;
uint16_t ys = 2;
uint16_t xm1, ym1, xm2, ym2;
uint16_t xem1, yem1, xem2, yem2;
uint16_t shot_tim = 0;
uint16_t enemy_shot_tim = 0;
int direction = 1;
uint16_t xe, ye, isdead = 1, Pisdead = 0;


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void disp_row(int row);
void draw_game();
void game_over();
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
  MX_SPI2_Init();
  MX_TIM1_Init();
  HAL_TIM_Base_Start_IT(&htim1);
  /* USER CODE BEGIN 2 */
  HAL_GPIO_WritePin(nOE_GPIO_Port, nOE_Pin, GPIO_PIN_RESET);
  srand(time(NULL));
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {
		if (HAL_GPIO_ReadPin(button1_GPIO_Port, button1_Pin))
		{
			direction = -1;
		}
		else
		{
			direction = 1;
		}
// внутри ветвления изменения 60 раз в 1 секунду
		if ((flag == 1) &&(Pisdead == 0)){
			enemy_shot_tim++;
			shot_tim++;
			ys = ys + direction;
			int arr_xs[12] = {xs, xs, xs+1, xs+2, xs+1, xs+2, xs+3, xs+4, xs+2, xs+1, xs, xs};
			int arr_ys[12] = {ys-2, ys-1,  ys-1, ys-1, ys, ys, ys, ys, ys+1, ys+1, ys+1, ys+2};
			if (ys > 13) ys=13;
			if (ys < 2) ys = 2;
			//стрельба
			if (shot_tim == 10) {
				xm2 = xm1;
				ym2 = ym1;
				xm1 = xs + 4;
				ym1 = ys;
				shot_tim = 0;
			}
			if ((!isdead) && (enemy_shot_tim == 20)) {
				xem2 = xem1;
				yem2 = yem1;
				xem1 = xe;
				yem1 = ye + 1;
				enemy_shot_tim = 0;
			}
			//координаты выстрелов
			xm1 = xm1 + 2;
			xm2 = xm2 + 2;
			xem1--;
			xem2--;

			if (isdead){
				uint16_t CordRand = rand()%3+1;
				if (CordRand == 1){
					xe = 28;
					ye = 2;
				}
				else if (CordRand == 2){
					xe = 28;
					ye = 6;
				}
				else if (CordRand == 3){
					xe = 28;
					ye = 11;
				}
				isdead = 0;
			}

			if ((xm2+1>=xe)&&(ym2>=ye)&&(ym2<=ye+2)){
				isdead = 1;
			}

			for (int i = 0; i <5; i++){
				if ((xem2 == arr_xs[i])&&(yem2==arr_ys[i])){
					Pisdead = 1;
					break;
				}
			}

			flag = 0;
		}
		if (Pisdead == 0){
			draw_game();
		}
		if (Pisdead){
			game_over();
		}

		disp1color_UpdateFromBuff(); //Перевод рассчитанных данных в массив
		prepare_data(); //Разбиение массива на массивы под каждую строку

		//Включение матрицы (построчно)
		for (uint8_t i = 0; i < 20; i++) {
			disp_row(0);
			disp_row(1);
			disp_row(2);
			disp_row(3);
		}
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

  /* USER CODE END 3 */

/**
  * @brief System Clock Configuration
  * @retval None
  */

/* USER CODE BEGIN 4 */

/**
  * @brief System Clock Configuration
  * @retval None
  */


/* USER CODE BEGIN 4 */

  void disp_row(int row){


  	  if (row == 0){

  		  for(uint8_t i=0; i<6; i++){
  		  		HAL_SPI_Transmit(&hspi2, &data1, 16, 10);
  		  }

  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

  		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_RESET);
  		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_RESET);
  	  }
  	  if (row == 1){

  		  for(uint8_t i=0; i<6; i++){
  		  		HAL_SPI_Transmit(&hspi2, &data2, 16, 10);
  		  }

  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

  		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
  		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_RESET);
  	  }

  	  if (row == 2){

  		  for(uint8_t i=0; i<6; i++){
  		  		HAL_SPI_Transmit(&hspi2, &data3, 16, 10);
  		  }

  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

  		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_RESET);
  		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
  	  }

  	  if (row == 3){

  		  for(uint8_t i=0; i<6; i++){
  		  		HAL_SPI_Transmit(&hspi2, &data4, 16, 10);
  		  }

  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_RESET);
  		  HAL_GPIO_WritePin(SCLK_GPIO_Port, SCLK_Pin, GPIO_PIN_SET);

  		  HAL_GPIO_WritePin(A_GPIO_Port, A_Pin, GPIO_PIN_SET);
  		  HAL_GPIO_WritePin(B_GPIO_Port, B_Pin, GPIO_PIN_SET);
  	  }


  	  HAL_GPIO_WritePin(nOE_GPIO_Port, nOE_Pin, GPIO_PIN_SET);
  		  for(uint32_t x=0; x<=500; x++) {};
  	 HAL_GPIO_WritePin(nOE_GPIO_Port, nOE_Pin, GPIO_PIN_RESET);
    }
void draw_game() {
	disp1color_FillScreenbuff(0); //очищение экрана перед выводом новых фигур

	disp1color_DrawLine((int) xm1, (int) ym1, (int) xm1 + 1, (int) ym1); //отрисовка снарядов нашего корабля
	disp1color_DrawLine((int) xm2, (int) ym2, (int) xm2 + 1, (int) ym2);

	disp1color_DrawPixel(xem1, yem1, 1); //отрисовка вражеских снарядов
	disp1color_DrawPixel(xem2, yem2, 1);

	disp1color_DrawRectangle(xe, ye, xe + 2, ye + 2); //отрисовка вражеского прямоугольника

	disp1color_DrawPixel((int) xs, (int) ys - 2, 1);
	disp1color_DrawLine((int) xs, (int) ys - 1, (int) xs + 2, (int) ys - 1); //отрисовка данных для корабля
	disp1color_DrawLine((int) xs + 1, (int) ys, (int) xs + 4, (int) ys);     //под отрисовкой понимается расчёт данных для массива
	disp1color_DrawLine((int) xs, (int) ys + 1, (int) xs + 2, (int) ys + 1); //который позже пойдёт в регистры матрицы и будет выводить изображение
	disp1color_DrawPixel((int) xs, (int) ys + 2, 1);
}
void game_over() {
	disp1color_FillScreenbuff(0);

	disp1color_DrawString(7, 0, FONTID_6X8M, pMyStr);
	disp1color_DrawString(5, 8, FONTID_6X8M, pMyStr2);
}
  /* USER CODE END 3 */

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
