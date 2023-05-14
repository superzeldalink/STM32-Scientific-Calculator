/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define STM32
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_RS_Pin GPIO_PIN_13
#define LCD_RS_GPIO_Port GPIOC
#define LCD_R_Pin GPIO_PIN_14
#define LCD_R_GPIO_Port GPIOC
#define LCD_E_Pin GPIO_PIN_15
#define LCD_E_GPIO_Port GPIOC
#define LCD_RST_Pin GPIO_PIN_2
#define LCD_RST_GPIO_Port GPIOA
#define ROW0_Pin GPIO_PIN_3
#define ROW0_GPIO_Port GPIOA
#define ROW1_Pin GPIO_PIN_4
#define ROW1_GPIO_Port GPIOA
#define ROW2_Pin GPIO_PIN_5
#define ROW2_GPIO_Port GPIOA
#define ROW3_Pin GPIO_PIN_6
#define ROW3_GPIO_Port GPIOA
#define ROW4_Pin GPIO_PIN_7
#define ROW4_GPIO_Port GPIOA
#define ROW5_Pin GPIO_PIN_0
#define ROW5_GPIO_Port GPIOB
#define ROW6_Pin GPIO_PIN_1
#define ROW6_GPIO_Port GPIOB
#define ROW7_Pin GPIO_PIN_2
#define ROW7_GPIO_Port GPIOB
#define COL0_Pin GPIO_PIN_12
#define COL0_GPIO_Port GPIOB
#define COL1_Pin GPIO_PIN_13
#define COL1_GPIO_Port GPIOB
#define COL2_Pin GPIO_PIN_14
#define COL2_GPIO_Port GPIOB
#define COL3_Pin GPIO_PIN_15
#define COL3_GPIO_Port GPIOB
#define COL4_Pin GPIO_PIN_8
#define COL4_GPIO_Port GPIOA
#define STATUS_LED_Pin GPIO_PIN_15
#define STATUS_LED_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
