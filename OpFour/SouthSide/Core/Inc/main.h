/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

//#include "../../CasualNoises/CasualNoises.h"

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

enum eStatusCodes
{
	scStartingUp		= 1,
//	scWaitingHandshake,
	scHandshakeComplete,
	scReady,

};

void displayStatus(uint32_t status);

void Error_Handler(void);

void* getTLV_DriverPtr();

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define INTERCONN_1_C_Pin GPIO_PIN_2
#define INTERCONN_1_C_GPIO_Port GPIOE
#define INTERCONN_2_A_Pin GPIO_PIN_3
#define INTERCONN_2_A_GPIO_Port GPIOE
#define INTERCONN_2_B_Pin GPIO_PIN_4
#define INTERCONN_2_B_GPIO_Port GPIOE
#define INTERCONN_2_C_Pin GPIO_PIN_5
#define INTERCONN_2_C_GPIO_Port GPIOE
#define TIME_MARKER_1_Pin GPIO_PIN_13
#define TIME_MARKER_1_GPIO_Port GPIOC
#define TIME_MARKER_2_Pin GPIO_PIN_14
#define TIME_MARKER_2_GPIO_Port GPIOC
#define TIME_MARKER_3_Pin GPIO_PIN_0
#define TIME_MARKER_3_GPIO_Port GPIOF
#define TIME_MARKER_4_Pin GPIO_PIN_1
#define TIME_MARKER_4_GPIO_Port GPIOF
#define IN_CV_3_Pin GPIO_PIN_3
#define IN_CV_3_GPIO_Port GPIOF
#define IN_CV_2_Pin GPIO_PIN_5
#define IN_CV_2_GPIO_Port GPIOF
#define IN_CV_1_Pin GPIO_PIN_0
#define IN_CV_1_GPIO_Port GPIOC
#define IN_1V_OCT_Pin GPIO_PIN_1
#define IN_1V_OCT_GPIO_Port GPIOC
#define SS_FLASH_CLK_Pin GPIO_PIN_5
#define SS_FLASH_CLK_GPIO_Port GPIOA
#define SS_FLASH_MISO_Pin GPIO_PIN_6
#define SS_FLASH_MISO_GPIO_Port GPIOA
#define SS_FLASH_MOSI_Pin GPIO_PIN_7
#define SS_FLASH_MOSI_GPIO_Port GPIOA
#define FLASH_CS_8_Pin GPIO_PIN_4
#define FLASH_CS_8_GPIO_Port GPIOC
#define FLASH_CS_4_Pin GPIO_PIN_5
#define FLASH_CS_4_GPIO_Port GPIOC
#define STATUS_LED_1_Pin GPIO_PIN_0
#define STATUS_LED_1_GPIO_Port GPIOB
#define STATUS_LED_2_Pin GPIO_PIN_2
#define STATUS_LED_2_GPIO_Port GPIOB
#define STATUS_LED_3_Pin GPIO_PIN_12
#define STATUS_LED_3_GPIO_Port GPIOF
#define STATUS_LED_4_Pin GPIO_PIN_15
#define STATUS_LED_4_GPIO_Port GPIOF
#define FLASH_CS_6_Pin GPIO_PIN_0
#define FLASH_CS_6_GPIO_Port GPIOG
#define FLASH_CS_2_Pin GPIO_PIN_7
#define FLASH_CS_2_GPIO_Port GPIOE
#define FLASH_CS_3_Pin GPIO_PIN_8
#define FLASH_CS_3_GPIO_Port GPIOE
#define FLASH_CS_7_Pin GPIO_PIN_12
#define FLASH_CS_7_GPIO_Port GPIOE
#define FLASH_CS_1_Pin GPIO_PIN_13
#define FLASH_CS_1_GPIO_Port GPIOE
#define FLASH_CS_5_Pin GPIO_PIN_14
#define FLASH_CS_5_GPIO_Port GPIOE
#define INTERCONN_SCK_Pin GPIO_PIN_13
#define INTERCONN_SCK_GPIO_Port GPIOB
#define INTERCONN_MISO_Pin GPIO_PIN_14
#define INTERCONN_MISO_GPIO_Port GPIOB
#define INTERCONN_MOSI_Pin GPIO_PIN_15
#define INTERCONN_MOSI_GPIO_Port GPIOB
#define INTERCONN_A_Pin GPIO_PIN_8
#define INTERCONN_A_GPIO_Port GPIOD
#define INTERCONN_A_EXTI_IRQn EXTI9_5_IRQn
#define INTERCONN_B_Pin GPIO_PIN_9
#define INTERCONN_B_GPIO_Port GPIOD
#define INTERCONN_C_Pin GPIO_PIN_10
#define INTERCONN_C_GPIO_Port GPIOD
#define INTERCONN_C_EXTI_IRQn EXTI15_10_IRQn
#define USB_D__Pin GPIO_PIN_11
#define USB_D__GPIO_Port GPIOA
#define USB_D_A12_Pin GPIO_PIN_12
#define USB_D_A12_GPIO_Port GPIOA
#define EX_HEARTH_BEAT_Pin GPIO_PIN_15
#define EX_HEARTH_BEAT_GPIO_Port GPIOA
#define NERVENET_1_Pin GPIO_PIN_3
#define NERVENET_1_GPIO_Port GPIOB
#define NERVENET_6_Pin GPIO_PIN_4
#define NERVENET_6_GPIO_Port GPIOB
#define NERVENET_2_Pin GPIO_PIN_5
#define NERVENET_2_GPIO_Port GPIOB
#define NERVENET_5_Pin GPIO_PIN_6
#define NERVENET_5_GPIO_Port GPIOB
#define NERVENET_3_Pin GPIO_PIN_7
#define NERVENET_3_GPIO_Port GPIOB
#define NERVENET_4_Pin GPIO_PIN_8
#define NERVENET_4_GPIO_Port GPIOB
#define INTERCONN_1_A_Pin GPIO_PIN_0
#define INTERCONN_1_A_GPIO_Port GPIOE
#define INTERCONN_1_B_Pin GPIO_PIN_1
#define INTERCONN_1_B_GPIO_Port GPIOE

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

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
