#include "hardware_interface/logging/serial_print.h"
#include "hardware_interface/system/clock.h"

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