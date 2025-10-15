# Debugging Logs

`printf` is diverted over UART for debugging purposes.
This file will go over how to debug the application using logs.

## Debugging steps

1. Connect the STM microcontroller via USB to the development device.
2. Debug the application. Preferred to use OpenOCD tool.
   - Use`src\configs\st_nucleo_f7.cfg` as the board config file if using the OpenOCD tool for debugging.
3. Check the COM port that the microcontroller is connected to on your device. Check on the device manager.
4. Create a serial connection to the COM port that the microcontroller is on. Preferably use Putty.
   - Baud rate: 115200
   - Word length: 8 bits
   - Stop bit: 1
   - Parity: None
   - HW flow: None
5. Run the debugger, and you should see log messages being piped to the terminal with the serial connection.
