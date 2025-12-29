/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STATUS_LED_3_Pin GPIO_PIN_5
#define STATUS_LED_3_GPIO_Port GPIOE
#define STATUS_LED_4_Pin GPIO_PIN_6
#define STATUS_LED_4_GPIO_Port GPIOE
#define TIME_MARKER_3_Pin GPIO_PIN_0
#define TIME_MARKER_3_GPIO_Port GPIOF
#define TIME_MARKER_4_Pin GPIO_PIN_1
#define TIME_MARKER_4_GPIO_Port GPIOF
#define TIME_MARKER_1_Pin GPIO_PIN_2
#define TIME_MARKER_1_GPIO_Port GPIOF
#define TIME_MARKER_2_Pin GPIO_PIN_3
#define TIME_MARKER_2_GPIO_Port GPIOF
#define FLASH_SCK_Pin GPIO_PIN_7
#define FLASH_SCK_GPIO_Port GPIOF
#define FLASH_MISO_Pin GPIO_PIN_8
#define FLASH_MISO_GPIO_Port GPIOF
#define FLASH_MOSI_Pin GPIO_PIN_9
#define FLASH_MOSI_GPIO_Port GPIOF
#define SLIDERS_S0_Pin GPIO_PIN_6
#define SLIDERS_S0_GPIO_Port GPIOA
#define SLIDERS_S1_Pin GPIO_PIN_7
#define SLIDERS_S1_GPIO_Port GPIOA
#define SLIDERS_S2_Pin GPIO_PIN_4
#define SLIDERS_S2_GPIO_Port GPIOC
#define EX_HEART_BEAT_Pin GPIO_PIN_13
#define EX_HEART_BEAT_GPIO_Port GPIOF
#define SER_OUT_RCLK_Pin GPIO_PIN_12
#define SER_OUT_RCLK_GPIO_Port GPIOB
#define SER_OUT_SCK_Pin GPIO_PIN_13
#define SER_OUT_SCK_GPIO_Port GPIOB
#define SER_OUT_SER_Pin GPIO_PIN_15
#define SER_OUT_SER_GPIO_Port GPIOB
#define POTS_S0_Pin GPIO_PIN_9
#define POTS_S0_GPIO_Port GPIOD
#define POTS_S1_Pin GPIO_PIN_10
#define POTS_S1_GPIO_Port GPIOD
#define POTS_S2_Pin GPIO_PIN_11
#define POTS_S2_GPIO_Port GPIOD
#define OLED_CS_Pin GPIO_PIN_1
#define OLED_CS_GPIO_Port GPIOD
#define OLED_DC_Pin GPIO_PIN_2
#define OLED_DC_GPIO_Port GPIOD
#define OLED_RES_Pin GPIO_PIN_3
#define OLED_RES_GPIO_Port GPIOD
#define FLASH_CS_2_Pin GPIO_PIN_4
#define FLASH_CS_2_GPIO_Port GPIOD
#define SER_IN_PL_Pin GPIO_PIN_6
#define SER_IN_PL_GPIO_Port GPIOD
#define SER_IN_Q7_Pin GPIO_PIN_9
#define SER_IN_Q7_GPIO_Port GPIOG
#define SER_IN_SCK_Pin GPIO_PIN_11
#define SER_IN_SCK_GPIO_Port GPIOG
#define FLASH_CS_1_Pin GPIO_PIN_12
#define FLASH_CS_1_GPIO_Port GPIOG
#define OLED_SCK_Pin GPIO_PIN_3
#define OLED_SCK_GPIO_Port GPIOB
#define OLED_MOSI_Pin GPIO_PIN_5
#define OLED_MOSI_GPIO_Port GPIOB
#define STATUS_LED_1_Pin GPIO_PIN_8
#define STATUS_LED_1_GPIO_Port GPIOB
#define STATUS_LED_2_Pin GPIO_PIN_9
#define STATUS_LED_2_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

void toggleTimeMarker_1();
void setTimeMarker_1();
void resetTimeMarker_1();

void toggleTimeMarker_2();
void setTimeMarker_2();
void resetTimeMarker_2();

void toggleTimeMarker_3();
void setTimeMarker_3();
void resetTimeMarker_3();

void toggleTimeMarker_4();
void setTimeMarker_4();
void resetTimeMarker_4();

enum eStatusCodes
{
	scStartingUp		= 1,
//	scWaitingHandshake,
//	scHandshakeComplete,
	scRunning,

};

void displayStatus(uint32_t status);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
