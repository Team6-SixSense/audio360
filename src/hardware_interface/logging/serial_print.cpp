#include "hardware_interface/logging/serial_print.h"
#include "hardware_interface/system/clock.h"

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

static void Error_Handler();

UART_HandleTypeDef UartHandle;

void setupLogging() {

  // STM32F7xx HAL library initialization. This sets up the microcontroller's
  // peripherals using the Hardware Abstraction Layer (HAL).
  HAL_Init();

  // Configure the system clock to 216 MHz
  //SystemClock_Config();

  // Initialize Board Support Language (BSP) for LED3
  BSP_LED_Init(LED3);

  /* Configure the UART peripheral. UART configured as follows:
      - Asynchronous mode (UART Mode)
      - Word Length = 8 Bits (7 data bit + 1 parity bit) : BE CAREFUL : Program
     7 data bits + 1 parity bit in PC
      - Stop Bit    = One Stop bit
      - Parity      = ODD parity
      - BaudRate    = 9600 baud  (9600 symbols per second)
      - Hardware flow control disabled (RTS and CTS signals)
  */
  UartHandle.Instance = USART3;

  UartHandle.Init.BaudRate = 9600;
  UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
  UartHandle.Init.StopBits = UART_STOPBITS_1;
  UartHandle.Init.Parity = UART_PARITY_ODD;
  UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  UartHandle.Init.Mode = UART_MODE_TX_RX;
  UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&UartHandle) != HAL_OK) {
    // Initialization Error.
    Error_Handler();
  }
}

static void Error_Handler() {
  // Turn LED3 on.
  BSP_LED_On(LED3);
}

/**
 * @brief  Retargets the C library printf function to the USART.
 * @param  None
 * @retval None
 */
PUTCHAR_PROTOTYPE {

  HAL_UART_Transmit(&UartHandle, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}