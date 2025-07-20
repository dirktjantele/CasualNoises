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

void Error_Handler(void);

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define OLED_CLK_Pin GPIO_PIN_2
#define OLED_CLK_GPIO_Port GPIOE
#define SER_IN_PL_Pin GPIO_PIN_3
#define SER_IN_PL_GPIO_Port GPIOE
#define SPI4_MISO_NC_Pin GPIO_PIN_5
#define SPI4_MISO_NC_GPIO_Port GPIOE
#define OLED_DIN_Pin GPIO_PIN_6
#define OLED_DIN_GPIO_Port GPIOE
#define OLED_RST_Pin GPIO_PIN_13
#define OLED_RST_GPIO_Port GPIOC
#define OLED_DC_Pin GPIO_PIN_14
#define OLED_DC_GPIO_Port GPIOC
#define OLED_CS_Pin GPIO_PIN_15
#define OLED_CS_GPIO_Port GPIOC
#define INTERCONN_C_Pin GPIO_PIN_0
#define INTERCONN_C_GPIO_Port GPIOF
#define INTERCONN_B_Pin GPIO_PIN_1
#define INTERCONN_B_GPIO_Port GPIOF
#define INTERCONN_B_EXTI_IRQn EXTI1_IRQn
#define INTERCONN_A_Pin GPIO_PIN_2
#define INTERCONN_A_GPIO_Port GPIOF
#define INTERCONN_SCK_Pin GPIO_PIN_7
#define INTERCONN_SCK_GPIO_Port GPIOF
#define INTERCONN_MISO_Pin GPIO_PIN_8
#define INTERCONN_MISO_GPIO_Port GPIOF
#define INTERCONN_MOSI_Pin GPIO_PIN_9
#define INTERCONN_MOSI_GPIO_Port GPIOF
#define POT_3_Pin GPIO_PIN_0
#define POT_3_GPIO_Port GPIOA
#define POT_4_Pin GPIO_PIN_2
#define POT_4_GPIO_Port GPIOA
#define POT_1_Pin GPIO_PIN_3
#define POT_1_GPIO_Port GPIOA
#define POT_2_Pin GPIO_PIN_4
#define POT_2_GPIO_Port GPIOA
#define FLASH_CS_4_Pin GPIO_PIN_5
#define FLASH_CS_4_GPIO_Port GPIOA
#define CODEC_MCLK_Pin GPIO_PIN_4
#define CODEC_MCLK_GPIO_Port GPIOC
#define CODEC_NRST_Pin GPIO_PIN_5
#define CODEC_NRST_GPIO_Port GPIOC
#define _1V_OCT_Pin GPIO_PIN_0
#define _1V_OCT_GPIO_Port GPIOB
#define CV_IN_1_Pin GPIO_PIN_1
#define CV_IN_1_GPIO_Port GPIOB
#define FLASH_CS_3_Pin GPIO_PIN_12
#define FLASH_CS_3_GPIO_Port GPIOF
#define CV_IN_2_Pin GPIO_PIN_13
#define CV_IN_2_GPIO_Port GPIOF
#define CV_IN_3_Pin GPIO_PIN_14
#define CV_IN_3_GPIO_Port GPIOF
#define STATUS_LED_4_Pin GPIO_PIN_0
#define STATUS_LED_4_GPIO_Port GPIOG
#define STATUS_LED_3_Pin GPIO_PIN_1
#define STATUS_LED_3_GPIO_Port GPIOG
#define STATUS_LED_2_Pin GPIO_PIN_7
#define STATUS_LED_2_GPIO_Port GPIOE
#define STATUS_LED_1_Pin GPIO_PIN_8
#define STATUS_LED_1_GPIO_Port GPIOE
#define FLASH_CS_2_Pin GPIO_PIN_14
#define FLASH_CS_2_GPIO_Port GPIOE
#define FLASH_CS_1_Pin GPIO_PIN_10
#define FLASH_CS_1_GPIO_Port GPIOH
#define NS_FLASH_SCK_Pin GPIO_PIN_13
#define NS_FLASH_SCK_GPIO_Port GPIOB
#define NS_FLASH_MISO_Pin GPIO_PIN_14
#define NS_FLASH_MISO_GPIO_Port GPIOB
#define NS_FLAS_MOSI_Pin GPIO_PIN_15
#define NS_FLAS_MOSI_GPIO_Port GPIOB
#define EX_HEART_BEAT_Pin GPIO_PIN_10
#define EX_HEART_BEAT_GPIO_Port GPIOA
#define USB_D__Pin GPIO_PIN_11
#define USB_D__GPIO_Port GPIOA
#define USB_D_A12_Pin GPIO_PIN_12
#define USB_D_A12_GPIO_Port GPIOA
#define CODEC_LRCK_Pin GPIO_PIN_15
#define CODEC_LRCK_GPIO_Port GPIOA
#define SER_IN_CP_Pin GPIO_PIN_10
#define SER_IN_CP_GPIO_Port GPIOC
#define SER_IN_Q7_Pin GPIO_PIN_11
#define SER_IN_Q7_GPIO_Port GPIOC
#define TIME_MARKER_1_Pin GPIO_PIN_0
#define TIME_MARKER_1_GPIO_Port GPIOD
#define TIME_MARKER_2_Pin GPIO_PIN_1
#define TIME_MARKER_2_GPIO_Port GPIOD
#define TIME_MARKER_3_Pin GPIO_PIN_2
#define TIME_MARKER_3_GPIO_Port GPIOD
#define TIME_MARKER_4_Pin GPIO_PIN_3
#define TIME_MARKER_4_GPIO_Port GPIOD
#define CODEC_SDOUT_Pin GPIO_PIN_7
#define CODEC_SDOUT_GPIO_Port GPIOD
#define CODEC_SDIN_Pin GPIO_PIN_9
#define CODEC_SDIN_GPIO_Port GPIOG
#define CODEC_CK_Pin GPIO_PIN_11
#define CODEC_CK_GPIO_Port GPIOG
#define SER_OUT_SCK_Pin GPIO_PIN_13
#define SER_OUT_SCK_GPIO_Port GPIOG
#define SER_OUT_SER_Pin GPIO_PIN_14
#define SER_OUT_SER_GPIO_Port GPIOG
#define SER_OUT_RCLK_Pin GPIO_PIN_7
#define SER_OUT_RCLK_GPIO_Port GPIOI

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
	scWaitingHandshake,
//	scHandshakeComplete,
	scRunning,

};
/*
void displayHandshakeCompleteStatus();
*/
void displayStatus(uint32_t status);

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
