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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "../../Library/KeyPad/KeyPad.h"
#include "../../Library/KeyPad/KeyPad.c"
#include "../../Library/ST7920_SERIAL/ST7920_SERIAL.h"
#include "../../Library/ST7920_SERIAL/ST7920_SERIAL.c"
#include "../../Library/EvalExpr/EvalExpr.h"
#include "../../Library/EvalExpr/EvalExpr.c"

#define ANSWER_ROW 7

TIM_HandleTypeDef htim1;

UART_HandleTypeDef huart1;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);

char input[256];
uint8_t input_ptr = 0;

uint8_t disX = 0, disY = 0;

void ClearRow(int row){
	GLCD_Font_Print(0, row, "                ");
}

void PrintChar(uint8_t ch){
	if(ch < 10) {
		char c[] = {0,0};
		c[0] = ch + 48;
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX++;
	} else if(ch == LN){
		char c[] = {'l','n',0};
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX+=2;
	} else if(ch == DERIVATIVE) {
		char c[] = {'d','(',0};
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX+=2;
	} else if(ch == LIMIT) {
		char c[] = {'l','(',0};
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX+=2;
	} else if (ch == SINE || ch == COSINE || ch == TANGENT || ch == ANSWER || ch == LOG) {
		char c[] = {0,0,0,0};
		switch(ch) {
		case SINE: c[0] = 's'; c[1] = 'i'; c[2] = 'n'; break;
		case COSINE: c[0] = 'c'; c[1] = 'o'; c[2] = 's'; break;
		case TANGENT: c[0] = 't'; c[1] = 'a'; c[2] = 'n'; break;
		case ANSWER: c[0] = 'A'; c[1] = 'n'; c[2] = 's'; break;
		case LOG: c[0] = 'l'; c[1] = 'o'; c[2] = 'g'; break;
		}
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX+=3;
	} else if(ch == LOGX) {
		char c[] = {'l','o','g','(',0};
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX+=4;
	} else {
		char c[] = {0,0};
		switch(ch) {
			case DOT: c[0] = '.'; break;
			case COMMA: c[0] = ','; break;
			case EQUAL: c[0] = '='; break;
			case PLUS: c[0] = '+'; break;
			case MINUS: c[0] = '-'; break;
			case MULTIPLY: c[0] = 'x'; break;
			case DIVIDE: c[0] = '/'; break;
			case BRACKET_OPEN: c[0] = '('; break;
			case BRACKET_CLOSE: c[0] = ')'; break;
			case EXPONENT: c[0] = '^'; break;
			case FACTORIAL: c[0] = '!'; break;
			case SQRT: c[0] = 128; break;
			case XRT: c[0] = 129; break;
			case X: c[0] = 130; break;
			case Y: c[0] = 131; break;
			case Z: c[0] = 132; break;
			case EQUAL_SIGN: c[0] = '='; break;
			case PINFTY: c[0] = 133; break;
			case NINFTY: c[0] = 134; break;
		}
		GLCD_Font_Print(disX, disY, (char*) &c);
		disX++;
	}
}

void BackspaceChar(int numChar){
	for(int i = 0; i < numChar; i++){
		disX--;
		GLCD_Font_Print(disX, disY, " ");
	}
}

void AddKey(uint8_t key){
	input[input_ptr++] = key;
	if(key == LOGX || key == DERIVATIVE || key == LIMIT)
		input[input_ptr++] = BRACKET_OPEN;

	PrintChar(key);
}

void BackSpace() {
  uint8_t prevKey = input[input_ptr - 1];
  input_ptr--;
  input[input_ptr] = 0;
  if(prevKey == SINE || prevKey == COSINE || prevKey == TANGENT || prevKey == ANSWER || prevKey == LOG)
	  BackspaceChar(3);
  else if (prevKey == LN)
	  BackspaceChar(2);
  else if (prevKey == BRACKET_OPEN) {
	  BackspaceChar(1);
	  if (input[input_ptr - 1] == LOGX)
		  BackspaceChar(3);
	  else if (input[input_ptr - 1] == LIMIT || input[input_ptr - 1] == DERIVATIVE)
		  BackspaceChar(1);
	  input_ptr--;
	  input[input_ptr] = 0;
  } else
	  BackspaceChar(1);
}

void PrintError(uint8_t errorCode){
	switch(errorCode){
	case 1:
	case 2:
	    GLCD_Font_Print(4, ANSWER_ROW, "Syntax error");
	    break;
	case 3:
	    GLCD_Font_Print(1, ANSWER_ROW, "Missing bracket");
	    break;
	case 4:
	    GLCD_Font_Print(7, ANSWER_ROW, "Div. by 0");
	    break;
	}
}

void PrintAnswer() {
	ClearRow(ANSWER_ROW);
	uint8_t errorCode = 0;
	double answer = evaluate(input, input_ptr, &errorCode);

	if(errorCode == 0){
	  char answerText[256] = "";
	  sprintf(answerText, "%0.10g", answer);
	  GLCD_Font_Print(16 - strlen(answerText), ANSWER_ROW, (char*) &answerText);
	} else {
	  PrintError(errorCode);
	}
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  KeyPad_Init();
  ST7920_Init();

  ST7920_GraphicMode(1);
  ST7920_Clear();

  InitSto();

  while (1) {
	  uint8_t key = KeyPad_WaitForKeyGetChar(0);
	  if(key != 0xFF){
		  switch(key) {
		  case BACKSPACE:
			  BackSpace();
			  break;

		  case EQUAL: {
			  PrintAnswer();
		      break;
		  }

		  case AC:
			  input_ptr = 0;
			  disX = 0;
			  disY = 0;
			  ST7920_Clear();
			  break;

		  case DOWN: {
			  uint8_t prevKey = input[input_ptr - 1];
			  BackSpace();

			  switch(prevKey){
			  case EXPONENT: AddKey(SQRT); break;
			  case SQRT: AddKey(XRT); break;
			  case XRT: AddKey(EXPONENT); break;

			  case SINE: AddKey(COSINE); break;
			  case COSINE: AddKey(TANGENT); break;
			  case TANGENT: AddKey(SINE); break;
			  case LN: AddKey(LOG); break;
			  case LOG: AddKey(LOGX); break;
			  case LOGX: AddKey(LN); break;

			  case DOT: AddKey(COMMA); break;
			  case COMMA: AddKey(DOT); break;

			  case X: AddKey(Y); break;
			  case Y: AddKey(Z); break;
			  case Z: AddKey(X); break;

			  case LIMIT: AddKey(DERIVATIVE); break;
			  case DERIVATIVE: AddKey(LIMIT); break;

			  case ZERO: AddKey(PINFTY); break;
			  case PINFTY: AddKey(NINFTY); break;
			  case NINFTY: AddKey(ZERO); break;
			  default: AddKey(prevKey); break;
			  }
			  break;
		  }

		  case S2D:{
			  ClearRow(ANSWER_ROW);
			  Fraction frac = to_fraction(answer);
			  char answerText[256] = "";
			  sprintf(answerText, "%ld/%ld", frac.num * frac.sign, frac.den);
		      GLCD_Font_Print(16 - strlen(answerText), ANSWER_ROW, (char*) &answerText);
			  break;
		  }

		  default:
			  AddKey(key);
			  break;
		  }

	      ST7920_Update();
	  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 16-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 0xffff-1;
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
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA7 PA8 PA9 PA10
                           PA11 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB3 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
