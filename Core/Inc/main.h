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
#include "stm32f1xx_hal.h"

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

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define intLED_Pin GPIO_PIN_13
#define intLED_GPIO_Port GPIOC
#define INT_1_Pin GPIO_PIN_4
#define INT_1_GPIO_Port GPIOA
#define INT_2_Pin GPIO_PIN_5
#define INT_2_GPIO_Port GPIOA
#define INT_3_Pin GPIO_PIN_0
#define INT_3_GPIO_Port GPIOB
#define INT_4_Pin GPIO_PIN_1
#define INT_4_GPIO_Port GPIOB
#define Button_Press_Pin GPIO_PIN_12
#define Button_Press_GPIO_Port GPIOB
#define ECHO_FRONT_Pin GPIO_PIN_15
#define ECHO_FRONT_GPIO_Port GPIOA
#define TRIG_FRONT_Pin GPIO_PIN_3
#define TRIG_FRONT_GPIO_Port GPIOB
#define ECHO_LEFT_Pin GPIO_PIN_4
#define ECHO_LEFT_GPIO_Port GPIOB
#define TRIG_LEFT_Pin GPIO_PIN_5
#define TRIG_LEFT_GPIO_Port GPIOB
#define ECHO_RIGHT_Pin GPIO_PIN_6
#define ECHO_RIGHT_GPIO_Port GPIOB
#define TRIG_RIGHT_Pin GPIO_PIN_7
#define TRIG_RIGHT_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
