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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <UI_Definitions.h>
#include <YellowPages.h>

#include "CasualNoises.h"
#include "SystemConfig.h"

#include "UI_Thread.h"

//#include <NerveNet/NerveNetMessage.h

#include "NorthSideConnection.h"

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
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc2;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;
SPI_HandleTypeDef hspi3;
SPI_HandleTypeDef hspi4;
SPI_HandleTypeDef hspi5;
DMA_HandleTypeDef hdma_spi2_tx;
DMA_HandleTypeDef hdma_spi4_rx;
DMA_HandleTypeDef hdma_spi4_tx;

TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim15;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MPU_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI2_Init(void);
static void MX_SPI3_Init(void);
static void MX_SPI5_Init(void);
static void MX_TIM4_Init(void);
static void MX_SPI1_Init(void);
static void MX_ADC2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM15_Init(void);
static void MX_SPI4_Init(void);
void StartDefaultTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint32_t gInitFreeHeapSize;
uint32_t getInitFreeHeapSize ()
{
	return gInitFreeHeapSize;
}

/*
 * displayStatus()
 */
void displayStatus(eStatusCodes status)
{
	uint32_t code = (uint32_t)status;
	HAL_GPIO_WritePin(GPIOB, STATUS_LED_2_Pin|STATUS_LED_1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOE, STATUS_LED_4_Pin|STATUS_LED_3_Pin, GPIO_PIN_SET);
	if (code & 0x00000001)
		HAL_GPIO_WritePin(GPIOB, STATUS_LED_1_Pin, GPIO_PIN_RESET);
	if (code & 0x00000002)
		HAL_GPIO_WritePin(GPIOB, STATUS_LED_2_Pin, GPIO_PIN_RESET);
	if (code & 0x00000004)
		HAL_GPIO_WritePin(GPIOE, STATUS_LED_3_Pin, GPIO_PIN_RESET);
	if (code & 0x00000008)
		HAL_GPIO_WritePin(GPIOE, STATUS_LED_4_Pin, GPIO_PIN_RESET);
}

/*
 * Time marker functions used for performance measurement using a logic analyzer
 */
void toggleTimeMarker_1()
{
	HAL_GPIO_TogglePin(GPIOF, TIME_MARKER_1_Pin);
}

void setTimeMarker_1()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_1_Pin, GPIO_PIN_SET);
}

void resetTimeMarker_1()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_1_Pin, GPIO_PIN_RESET);
}

void toggleTimeMarker_2()
{
	HAL_GPIO_TogglePin(GPIOF, TIME_MARKER_2_Pin);
}

void setTimeMarker_2()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_2_Pin, GPIO_PIN_SET);
}

void resetTimeMarker_2()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_2_Pin, GPIO_PIN_RESET);
}

void toggleTimeMarker_3()
{
	HAL_GPIO_TogglePin(GPIOF, TIME_MARKER_3_Pin);
}

void setTimeMarker_3()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_3_Pin, GPIO_PIN_SET);
}

void resetTimeMarker_3()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_3_Pin, GPIO_PIN_RESET);
}

void toggleTimeMarker_4()
{
	HAL_GPIO_TogglePin(GPIOF, TIME_MARKER_4_Pin);
}

void setTimeMarker_4()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_4_Pin, GPIO_PIN_SET);
}

void resetTimeMarker_4()
{
	HAL_GPIO_WritePin(GPIOF, TIME_MARKER_4_Pin, GPIO_PIN_RESET);
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

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

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
  MX_DMA_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
  MX_SPI5_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_ADC2_Init();
  MX_TIM6_Init();
  MX_TIM15_Init();
  MX_SPI4_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 34;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
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

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV8;
  hadc2.Init.Resolution = ADC_RESOLUTION_16B;
  hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 2;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DMA_ONESHOT;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc2.Init.OversamplingMode = DISABLE;
  hadc2.Init.Oversampling.Ratio = 1;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_15;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_810CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_19;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI2_Init(void)
{

  /* USER CODE BEGIN SPI2_Init 0 */

  /* USER CODE END SPI2_Init 0 */

  /* USER CODE BEGIN SPI2_Init 1 */

  /* USER CODE END SPI2_Init 1 */
  /* SPI2 parameter configuration*/
  hspi2.Instance = SPI2;
  hspi2.Init.Mode = SPI_MODE_MASTER;
  hspi2.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi2.Init.NSS = SPI_NSS_SOFT;
  hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi2.Init.CRCPolynomial = 0x0;
  hspi2.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi2.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi2.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi2.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi2.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi2.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi2.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi2.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi2.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI2_Init 2 */

  /* USER CODE END SPI2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES_TXONLY;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 0x0;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi3.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi3.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi3.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi3.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi3.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi3.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi3.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi3.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief SPI4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  /* SPI4 parameter configuration*/
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 0x0;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 0x0;
  hspi5.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi5.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi5.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi5.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi5.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi5.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi5.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi5.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

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
  htim4.Init.Prescaler = 120 - 1;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 1000 - 1;
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
  htim6.Init.Prescaler = 120 - 1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
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
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(void)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = 24000 - 1;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = 100 - 1;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
  /* DMA1_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
  /* DMA1_Stream3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, STATUS_LED_3_Pin|STATUS_LED_4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, TIME_MARKER_3_Pin|TIME_MARKER_4_Pin|TIME_MARKER_1_Pin|TIME_MARKER_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SLIDERS_S0_Pin|SLIDERS_S1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SLIDERS_S2_GPIO_Port, SLIDERS_S2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NERVE_NET_RESET_GPIO_Port, NERVE_NET_RESET_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(EX_HEART_BEAT_GPIO_Port, EX_HEART_BEAT_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(NERVE_NET_REQ_GPIO_Port, NERVE_NET_REQ_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SER_OUT_RCLK_Pin|STATUS_LED_1_Pin|STATUS_LED_2_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, POTS_S0_Pin|POTS_S1_Pin|POTS_S2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, OLED_CS_Pin|OLED_DC_Pin|OLED_RES_Pin|FLASH_CS_2_Pin
                          |SER_IN_PL_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(FLASH_CS_1_GPIO_Port, FLASH_CS_1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : STATUS_LED_3_Pin STATUS_LED_4_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_3_Pin|STATUS_LED_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : TIME_MARKER_3_Pin TIME_MARKER_4_Pin TIME_MARKER_1_Pin TIME_MARKER_2_Pin */
  GPIO_InitStruct.Pin = TIME_MARKER_3_Pin|TIME_MARKER_4_Pin|TIME_MARKER_1_Pin|TIME_MARKER_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pins : SLIDERS_S0_Pin SLIDERS_S1_Pin */
  GPIO_InitStruct.Pin = SLIDERS_S0_Pin|SLIDERS_S1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SLIDERS_S2_Pin */
  GPIO_InitStruct.Pin = SLIDERS_S2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SLIDERS_S2_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : NERVE_NET_RESET_Pin SER_OUT_RCLK_Pin */
  GPIO_InitStruct.Pin = NERVE_NET_RESET_Pin|SER_OUT_RCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : EX_HEART_BEAT_Pin */
  GPIO_InitStruct.Pin = EX_HEART_BEAT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(EX_HEART_BEAT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : NERVE_NET_ACK_Pin */
  GPIO_InitStruct.Pin = NERVE_NET_ACK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(NERVE_NET_ACK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : NERVE_NET_REQ_Pin */
  GPIO_InitStruct.Pin = NERVE_NET_REQ_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(NERVE_NET_REQ_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : POTS_S0_Pin POTS_S1_Pin POTS_S2_Pin OLED_CS_Pin
                           OLED_DC_Pin OLED_RES_Pin FLASH_CS_2_Pin SER_IN_PL_Pin */
  GPIO_InitStruct.Pin = POTS_S0_Pin|POTS_S1_Pin|POTS_S2_Pin|OLED_CS_Pin
                          |OLED_DC_Pin|OLED_RES_Pin|FLASH_CS_2_Pin|SER_IN_PL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : FLASH_CS_1_Pin */
  GPIO_InitStruct.Pin = FLASH_CS_1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(FLASH_CS_1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : STATUS_LED_1_Pin STATUS_LED_2_Pin */
  GPIO_InitStruct.Pin = STATUS_LED_1_Pin|STATUS_LED_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(NERVE_NET_ACK_EXTI_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(NERVE_NET_ACK_EXTI_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */

	// Sweep the status led's as a sign of live...
	displayStatus((eStatusCodes)0x01);
	osDelay(100);
	displayStatus((eStatusCodes)0x02);
	osDelay(100);
	displayStatus((eStatusCodes)0x04);
	osDelay(100);
	displayStatus((eStatusCodes)0x08);
	osDelay(100);
	displayStatus(scStartingUp);

    // Get the amount of free heap memory
    size_t xFreeHeapSize = xPortGetFreeHeapSize();
    gInitFreeHeapSize = xFreeHeapSize;

    // Create a trigger thread to toggle TimeMarker_1
    BaseType_t res;
    TaskHandle_t xHandle;
	res = xTaskCreate(CasualNoises::TriggerThread, "TriggerThread", 32, nullptr,
			TRIGGER_THREAD_PRIORITY, &xHandle);
	UNUSED(res);

	// Create a signature map for all encoders/switches
	uint32_t num;
	constexpr uint32_t noOfEncoders 	= 1 + 7;			// 1 encoder & 7 switches
	static CasualNoises::sEncoderSignature encoderSignatures[noOfEncoders];
	num = 0;												// Encoder #1 (Value)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 0;
	encoderSignatures[num].switchPinNo	= 6;
	encoderSignatures[num].enc_A_DevNo	= 0;
	encoderSignatures[num].enc_A_PinNo	= 2;
	encoderSignatures[num].enc_B_DevNo	= 0;
	encoderSignatures[num].enc_B_PinNo	= 3;
	++num;													// Switch #1 (Alt)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 0;
	encoderSignatures[num].switchPinNo	= 1;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;
	++num;													// Switch #2 (<)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 0;
	encoderSignatures[num].switchPinNo	= 7;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;
	++num;													// Switch #3 (>)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 1;
	encoderSignatures[num].switchPinNo	= 0;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;
	++num;													// Switch #4 (Setup)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 1;
	encoderSignatures[num].switchPinNo	= 1;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;
	++num;													// Switch #5 (Load)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 1;
	encoderSignatures[num].switchPinNo	= 2;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;
	++num;													// Switch #6 (Save)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 1;
	encoderSignatures[num].switchPinNo	= 3;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;
	++num;													// Switch #7 (Exit)
	encoderSignatures[num].encoderNo	= num;
	encoderSignatures[num].switchDevNo	= 0;
	encoderSignatures[num].switchPinNo	= 4;
	encoderSignatures[num].enc_A_DevNo	= 0xff;
	encoderSignatures[num].enc_A_PinNo	= 0xff;
	encoderSignatures[num].enc_B_DevNo	= 0xff;
	encoderSignatures[num].enc_B_PinNo	= 0xff;

	// Create a signature map for all encoders/switches
	constexpr uint32_t noADC_Multiplexers 	= 2;			// 2 multiplexers: 1 pots & 1 sliders
	static CasualNoises::sADC_MultiplexerSignature sADC_MultiplexerSignatures[noADC_Multiplexers];
	num = 0;												// Potentiometer multiplexer
	sADC_MultiplexerSignatures[num].SO_Pin	= POTS_S0_Pin;
	sADC_MultiplexerSignatures[num].SO_Port	= GPIOD;
	sADC_MultiplexerSignatures[num].S1_Pin	= POTS_S1_Pin;
	sADC_MultiplexerSignatures[num].S1_Port	= GPIOD;
	sADC_MultiplexerSignatures[num].S2_Pin	= POTS_S2_Pin;
	sADC_MultiplexerSignatures[num].S2_Port	= GPIOD;
	sADC_MultiplexerSignatures[num].mask	= 0x56;
	++num;													// Slider multiplexer
	sADC_MultiplexerSignatures[num].SO_Pin	= SLIDERS_S0_Pin;
	sADC_MultiplexerSignatures[num].SO_Port	= GPIOA;
	sADC_MultiplexerSignatures[num].S1_Pin	= SLIDERS_S1_Pin;
	sADC_MultiplexerSignatures[num].S1_Port	= GPIOA;
	sADC_MultiplexerSignatures[num].S2_Pin	= SLIDERS_S2_Pin;
	sADC_MultiplexerSignatures[num].S2_Port	= SLIDERS_S2_GPIO_Port;
	sADC_MultiplexerSignatures[num].mask	= 0xff;

	// Encoder #1 (Value)
	// Create a UI thread and run it
	// Note, part of the UI_ThreadData is filled in by the UI thread when starting up
	void ( *nerveNetCallBackPtr ) ( uint32_t size, uint8_t* ptr ) = nullptr;
	static CasualNoises::UI_ThreadData uiData;
	// ... NVM Driver settings
	uiData.nvmDriverInitData.noOfDevices				= 2;
	uiData.nvmDriverInitData.hspix_ptr					= &hspi5;
	uiData.nvmDriverInitData.deviceSelectPorts[0]		= FLASH_CS_1_GPIO_Port;
	uiData.nvmDriverInitData.deviceSelectPins[0]		= FLASH_CS_1_Pin;
	uiData.nvmDriverInitData.deviceSelectPorts[1]		= FLASH_CS_2_GPIO_Port;
	uiData.nvmDriverInitData.deviceSelectPins[1]		= FLASH_CS_2_Pin;
	// ... encoder thread settings
	uiData.spiEncoderThreadData.encoderSignatureArray	= encoderSignatures; // Encoder signature array
	uiData.spiEncoderThreadData.noOfEncoders			= noOfEncoders;
	uiData.spiEncoderThreadData.encoder_SPI_Handle		= &hspi1;			// SPI1 is used for encoders and switches
	uiData.spiEncoderThreadData.serInPL_Port			= GPIOD;			// Port and pin for parallel load of shift registers
	uiData.spiEncoderThreadData.serInPL_Pin				= SER_IN_PL_Pin;
	uiData.spiEncoderThreadData.noOfDevices				= 2;				// No of available shift registers
	// ... led thread settings
	uiData.spi_LED_ThreadData.LED_SPI_Handle			= &hspi2;
	uiData.spi_LED_ThreadData.TimerHandle				= &htim4;
	uiData.spi_LED_ThreadData.SER_OUT_RCLK_GPIO__Port	= SER_OUT_RCLK_GPIO_Port;
	uiData.spi_LED_ThreadData.SER_OUT_RCLK_GPIO_Pin		= SER_OUT_RCLK_Pin;
	// ... oled display settings
	uiData.oledConfigData.SPI							= &hspi3;			// SPI3 drives the oled display
	uiData.oledConfigData.CS_PIN						= OLED_CS_Pin;
	uiData.oledConfigData.CS_PORT						= GPIOD;
	uiData.oledConfigData.DC_PIN						= OLED_DC_Pin;
	uiData.oledConfigData.DC_PORT						= GPIOD;
	uiData.oledConfigData.NRST_PIN						= OLED_RES_Pin;
	uiData.oledConfigData.NRST_PORT						= GPIOD;
	// ...
	uiData.ADC_MultiplexerThreadData.hadc				= &hadc2;
	uiData.ADC_MultiplexerThreadData.noOfMultiplexers	= 2;
	uiData.ADC_MultiplexerThreadData.multiplexerSignatureArray = sADC_MultiplexerSignatures;

	// Start UI thread
	uiData.nerveNetCallBackPtr   						= &nerveNetCallBackPtr;
	res = CasualNoises::Start_UI_Thread ( &uiData );
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::FreeRTOS_ErrorRes);

	// There are no NerveNet threads running
	for (uint32_t i = 0; i < MAX_NO_OF_NERVENET_MASTER_THREADS; ++i)
		gNerveNetMasterThreadPtr[i] = nullptr;
	for (uint32_t i = 0; i < MAX_NO_OF_NERVENET_SLAVE_THREADS; ++i)
		gNerveNetSlaveThreadPtr[i] = nullptr;

	// Start NerveNet master thread
	static CasualNoises::sNerveNetThreadData nerveNetThreadData;
	memset(&nerveNetThreadData, 0x00, sizeof(CasualNoises::sNerveNetThreadData));
	nerveNetThreadData.NerveNetThreadNo			= 0;
	nerveNetThreadData.NerveNet_REQ_Port		= NERVE_NET_REQ_GPIO_Port;
	nerveNetThreadData.NerveNet_REQ_Pin			= NERVE_NET_REQ_Pin;
	nerveNetThreadData.NerveNet_ACK_Port		= NERVE_NET_ACK_GPIO_Port;
	nerveNetThreadData.NerveNet_ACK_Pin			= NERVE_NET_ACK_Pin;
	nerveNetThreadData.NerveNet_RESET_Port		= NERVE_NET_RESET_GPIO_Port;
	nerveNetThreadData.NerveNet_RESET_Pin		= NERVE_NET_RESET_Pin;
	nerveNetThreadData.NerveNet_SPI_Ptr			= &hspi4;
	nerveNetThreadData.NerveNetThreadQueue		= &CasualNoises::gYellowPages.gNerveNetMasterThreadQueueHandle;
	nerveNetThreadData.NerveNetRunningFlagPtr	= &CasualNoises::gYellowPages.gNetMasterThreadRunning;
	static TaskHandle_t xHandlePtr;
	static CasualNoises::NerveNetMasterThread nerveNetMasterThread;
	res = CasualNoises::startNerveNetMasterThread ( &nerveNetMasterThread, &nerveNetThreadData, &xHandlePtr );
	if (res != pdPASS)
		CN_ReportFault(eErrorCodes::NerveNetThread_Error);
	CasualNoises::gYellowPages.gNerveNetMasterThreadTaskHandle 	= xHandlePtr;
	CasualNoises::NorthSideConnection connection;
	nerveNetMasterThread.setNerveNetMasterProcessorPtr ( &connection );

   /* Infinite loop */
	osDelay(200);
	displayStatus(scRunning);
	for(;;)
	{
		HAL_GPIO_WritePin ( EX_HEART_BEAT_GPIO_Port, EX_HEART_BEAT_Pin, GPIO_PIN_RESET );
		osDelay (pdMS_TO_TICKS ( 100 ) );
		HAL_GPIO_WritePin ( EX_HEART_BEAT_GPIO_Port, EX_HEART_BEAT_Pin, GPIO_PIN_SET );
		osDelay (pdMS_TO_TICKS ( 100) );
		HAL_GPIO_WritePin ( EX_HEART_BEAT_GPIO_Port, EX_HEART_BEAT_Pin, GPIO_PIN_RESET );
		osDelay ( pdMS_TO_TICKS (100) );
		HAL_GPIO_WritePin ( EX_HEART_BEAT_GPIO_Port, EX_HEART_BEAT_Pin, GPIO_PIN_SET );
		osDelay( pdMS_TO_TICKS ( 700) );
	}

  /* USER CODE END 5 */
}

 /* MPU Configuration */

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};

  /* Disables the MPU */
  HAL_MPU_Disable();

  /** Initializes and configures the Region and the memory to be protected
  */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
	static uint32_t tim2Cnt = 0;								// ToDo remove debug code
	static uint32_t tim6Cnt = 0;

	if (htim->Instance == TIM2)
	{
	  ++tim2Cnt;
	}
	else if (htim->Instance == TIM4)
	{
	  CasualNoises::Handle_LED_RefreshInterrupts();
	}
	else if (htim->Instance == TIM6)
	{
		++tim6Cnt;
	}

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
