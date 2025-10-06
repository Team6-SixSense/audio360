#include "hardware_interface/logging/serial_print.h"
#include <unistd.h>   // for prototype of _write

extern UART_HandleTypeDef UartHandle;

int _write(int file, char *ptr, int len)
{
    // Send the characters via HAL_UART_Transmit
    HAL_UART_Transmit(&UartHandle, (uint8_t*)ptr, len, HAL_MAX_DELAY);

    return len;
}