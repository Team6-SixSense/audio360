/**
 ******************************************************************************
 * @file    pheripheral.cpp
 * @brief   Peripheral functions.
 ******************************************************************************
 */
#ifdef STM_BUILD
#include "peripheral.h"

#include "fatfs.h"
#include "usb_device.h"
#include "embedded_mic.h"

static void PeriphCommonClock_Config();
static void MX_GPIO_Init();
static void MX_SPI1_Init();
static void MX_USART3_UART_Init();

static void MX_DMA_Init();

UART_HandleTypeDef huart3;
SPI_HandleTypeDef SD_SPI_HANDLE;



#define SD_CS_Pin GPIO_PIN_4
#define SD_CS_GPIO_Port GPIOA
extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
void setupPeripherals() {
  // Configure the system clock to 216 MHz
  SystemClock_Config();
  PeriphCommonClock_Config();

  // STM32F7xx HAL library initialization. This sets up the microcontroller's
  // peripherals using the Hardware Abstraction Layer (HAL).
  HAL_Init();

  // Set buffer to flush immediately.
  setvbuf(stdout, NULL, _IONBF, 0);

  // Set up GPIO. Must call before setting up other peripherals.
  MX_GPIO_Init();

  // Initialize Board Support Language (BSP) for LED3
  BSP_LED_Init(LED3);

  // Set up and intial UART pherrials.
  MX_USART3_UART_Init();

  MX_DMA_Init();
  // Set up the microphone SAI peripherals
  embedded_mic_init();

  // Set up logging sd card and FATFS
  MX_SPI1_Init();
  MX_FATFS_Init();

  USB_OTG_FS->GUSBCFG |= USB_OTG_GUSBCFG_FDMOD;

  /* 1. Power up the transceiver (1 = Power On, 0 = Power Down) */
  USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_PWRDWN;

  /* 2. Disable VBUS sensing (tells the PHY to ignore the VBUS pin) */
  /* Try NOVBUSSENS if VBDEN doesn't compile */
  USB_OTG_FS->GCCFG |= USB_OTG_GCCFG_VBDEN;

  /* 3. Force B-Device Session (Ignore the physical ID pin grounded by the adapter) */
  USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOVAL;
  USB_OTG_FS->GOTGCTL |= USB_OTG_GOTGCTL_BVALOEN;

  MX_USB_DEVICE_Init();

  HAL_PCD_DevConnect(&hpcd_USB_OTG_FS);


}

/** @brief Sets up clock for the entire system. */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  /** Activate the Over-Drive mode */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK) {
    Error_Handler();
  }
}

/** @brief Sets up clock for specific periperals. */
void PeriphCommonClock_Config(void) {
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock */
  PeriphClkInitStruct.PeriphClockSelection =
      RCC_PERIPHCLK_SAI1 | RCC_PERIPHCLK_SAI2;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 3;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.Sai1ClockSelection = RCC_SAI1CLKSOURCE_PLLSAI;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
    Error_Handler();
  }
}

/** @brief Initializes GPIO. */
static void MX_GPIO_Init() {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SD_CS_GPIO_Port, SD_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : SD_CS_Pin */
  GPIO_InitStruct.Pin = SD_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(SD_CS_GPIO_Port, &GPIO_InitStruct);
}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);
  /* DMA2_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream6_IRQn);

}

/** @brief Initializes SPI 1 bus. */
static void MX_SPI1_Init() {
  /* SPI1 parameter configuration*/
  SD_SPI_HANDLE.Instance = SPI1;
  SD_SPI_HANDLE.Init.Mode = SPI_MODE_MASTER;
  SD_SPI_HANDLE.Init.Direction = SPI_DIRECTION_2LINES;
  SD_SPI_HANDLE.Init.DataSize = SPI_DATASIZE_8BIT;
  SD_SPI_HANDLE.Init.CLKPolarity = SPI_POLARITY_LOW;
  SD_SPI_HANDLE.Init.CLKPhase = SPI_PHASE_1EDGE;
  SD_SPI_HANDLE.Init.NSS = SPI_NSS_SOFT;
  SD_SPI_HANDLE.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  SD_SPI_HANDLE.Init.FirstBit = SPI_FIRSTBIT_MSB;
  SD_SPI_HANDLE.Init.TIMode = SPI_TIMODE_DISABLE;
  SD_SPI_HANDLE.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  SD_SPI_HANDLE.Init.CRCPolynomial = 7;
  SD_SPI_HANDLE.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  SD_SPI_HANDLE.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&SD_SPI_HANDLE) != HAL_OK) {
    Error_Handler();
  }
}



/** @brief Initializes USART3. */
static void MX_USART3_UART_Init() {
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

  if (HAL_UART_Init(&huart3) != HAL_OK) {
    Error_Handler();
  }
}

void Error_Handler() {
  // Turn LED3 on.
  BSP_LED_On(LED3);
  while (1) {
  }
}



#endif
