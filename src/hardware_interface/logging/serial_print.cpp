#include "hardware_interface/logging/serial_print.h"
#include "hardware_interface/system/clock.h"

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

static void Error_Handler();
static void MX_USART3_UART_Init();

UART_HandleTypeDef huart3;

void setupLogging() {

  // STM32F7xx HAL library initialization. This sets up the microcontroller's
  // peripherals using the Hardware Abstraction Layer (HAL).
  HAL_Init();

  // Configure the system clock to 216 MHz
  SystemClock_Config();

  // Set buffer to flush immediately.
  setvbuf(stdout, NULL, _IONBF, 0);

  // Initialize Board Support Language (BSP) for LED3
  BSP_LED_Init(LED3);

  // __HAL_RCC_GPIOB_CLK_ENABLE();

  // Set up and intial UART pherrials.
  MX_USART3_UART_Init();
}

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

static void Error_Handler() {
  // Turn LED3 on.
  BSP_LED_On(LED3);
  while (1) {
  }
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE {

  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}

// #define USER_Btn_Pin GPIO_PIN_13
// #define USER_Btn_GPIO_Port GPIOC
// #define MCO_Pin GPIO_PIN_0
// #define MCO_GPIO_Port GPIOH
// #define RMII_MDC_Pin GPIO_PIN_1
// #define RMII_MDC_GPIO_Port GPIOC
// #define RMII_REF_CLK_Pin GPIO_PIN_1
// #define RMII_REF_CLK_GPIO_Port GPIOA
// #define RMII_MDIO_Pin GPIO_PIN_2
// #define RMII_MDIO_GPIO_Port GPIOA
// #define RMII_CRS_DV_Pin GPIO_PIN_7
// #define RMII_CRS_DV_GPIO_Port GPIOA
// #define RMII_RXD0_Pin GPIO_PIN_4
// #define RMII_RXD0_GPIO_Port GPIOC
// #define RMII_RXD1_Pin GPIO_PIN_5
// #define RMII_RXD1_GPIO_Port GPIOC
// #define LD1_Pin GPIO_PIN_0
// #define LD1_GPIO_Port GPIOB
// #define RMII_TXD1_Pin GPIO_PIN_13
// #define RMII_TXD1_GPIO_Port GPIOB
// #define LD3_Pin GPIO_PIN_14
// #define LD3_GPIO_Port GPIOB
// #define STLK_RX_Pin GPIO_PIN_8
// #define STLK_RX_GPIO_Port GPIOD
// #define STLK_TX_Pin GPIO_PIN_9
// #define STLK_TX_GPIO_Port GPIOD
// #define USB_PowerSwitchOn_Pin GPIO_PIN_6
// #define USB_PowerSwitchOn_GPIO_Port GPIOG
// #define USB_OverCurrent_Pin GPIO_PIN_7
// #define USB_OverCurrent_GPIO_Port GPIOG
// #define USB_SOF_Pin GPIO_PIN_8
// #define USB_SOF_GPIO_Port GPIOA
// #define USB_VBUS_Pin GPIO_PIN_9
// #define USB_VBUS_GPIO_Port GPIOA
// #define USB_ID_Pin GPIO_PIN_10
// #define USB_ID_GPIO_Port GPIOA
// #define USB_DM_Pin GPIO_PIN_11
// #define USB_DM_GPIO_Port GPIOA
// #define USB_DP_Pin GPIO_PIN_12
// #define USB_DP_GPIO_Port GPIOA
// #define TMS_Pin GPIO_PIN_13
// #define TMS_GPIO_Port GPIOA
// #define TCK_Pin GPIO_PIN_14
// #define TCK_GPIO_Port GPIOA
// #define RMII_TX_EN_Pin GPIO_PIN_11
// #define RMII_TX_EN_GPIO_Port GPIOG
// #define RMII_TXD0_Pin GPIO_PIN_13
// #define RMII_TXD0_GPIO_Port GPIOG
// #define SWO_Pin GPIO_PIN_3
// #define SWO_GPIO_Port GPIOB
// #define LD2_Pin GPIO_PIN_7
// #define LD2_GPIO_Port GPIOB

// void HAL_MspInit(void) {

//   /* USER CODE BEGIN MspInit 0 */

//   /* USER CODE END MspInit 0 */

//   __HAL_RCC_PWR_CLK_ENABLE();
//   __HAL_RCC_SYSCFG_CLK_ENABLE();

//   /* System interrupt init*/

//   /* USER CODE BEGIN MspInit 1 */

//   /* USER CODE END MspInit 1 */
// }

// /**
//  * @brief UART MSP Initialization
//  * This function configures the hardware resources used in this example
//  * @param huart: UART handle pointer
//  * @retval None
//  */
// void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
//   GPIO_InitTypeDef GPIO_InitStruct = {0};
//   RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
//   if (huart->Instance == USART3) {
//     /* USER CODE BEGIN USART3_MspInit 0 */

//     /* USER CODE END USART3_MspInit 0 */

//     /** Initializes the peripherals clock
//      */
//     PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3;
//     PeriphClkInitStruct.Usart3ClockSelection = RCC_USART3CLKSOURCE_PCLK1;
//     if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
//       while (1) {
//       }
//     }

//     /* Peripheral clock enable */
//     __HAL_RCC_USART3_CLK_ENABLE();

//     __HAL_RCC_GPIOD_CLK_ENABLE();
//     /**USART3 GPIO Configuration
//     PD8     ------> USART3_TX
//     PD9     ------> USART3_RX
//     */
//     GPIO_InitStruct.Pin = STLK_RX_Pin | STLK_TX_Pin;
//     GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//     GPIO_InitStruct.Pull = GPIO_NOPULL;
//     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
//     GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
//     HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

//     /* USER CODE BEGIN USART3_MspInit 1 */

//     /* USER CODE END USART3_MspInit 1 */
//   }
// }

// /**
//  * @brief UART MSP De-Initialization
//  * This function freeze the hardware resources used in this example
//  * @param huart: UART handle pointer
//  * @retval None
//  */
// void HAL_UART_MspDeInit(UART_HandleTypeDef *huart) {
//   if (huart->Instance == USART3) {
//     /* USER CODE BEGIN USART3_MspDeInit 0 */

//     /* USER CODE END USART3_MspDeInit 0 */
//     /* Peripheral clock disable */
//     __HAL_RCC_USART3_CLK_DISABLE();

//     /**USART3 GPIO Configuration
//     PD8     ------> USART3_TX
//     PD9     ------> USART3_RX
//     */
//     HAL_GPIO_DeInit(GPIOD, STLK_RX_Pin | STLK_TX_Pin);

//     /* USER CODE BEGIN USART3_MspDeInit 1 */

//     /* USER CODE END USART3_MspDeInit 1 */
//   }
// }
