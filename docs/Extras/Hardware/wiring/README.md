# Wiring

This folder contains information on wiring hardware components onto the microcontroller.

## Microphone Array

Below is an wiring for the microphone array. This wiring assumes 4 microphones
are being use simultaneously.

![image](./microphone_array.png)

The microcontroller comes with 2 SAI (Serial Audio Interface), which is used as peripheral input
for microphones. Each SAI bus comes with a sub block which can support taking input up to 2 microphones.
For simplicity, each microphone is wired to a single sub block.

1. SAI 1 block A (Pin PE6)
2. SAI 1 block B (Pin PE3)
3. SAI 2 block A (Pin PD11)
4. SAI 2 block B (Pin PA0)

SAI 1 block A acts as the master block for synchronization purposes. All other blocks synchronize to this block.

### Microphone

Microphones have the following pins:

1. `VDD`: Voltage (3.3V)
2. `GND`: Ground
3. `SD`: Serial Data. This is the pin that the microphone sends audio data.
4. `SCK`: Serial Clock. This is the clock for the microphone.
5. `WS`: Word Selection. Clock line that indicates the start of a new audio frame.
6. `L\R`: Left or Right channel. Low is left channel, and high is right channel.

For four microphone synchronization, `L/R` input of all microphones must be the same.
This is because data is sampled on WS signal on either left or right channel. For example,
when WS = 0, left channel is sampled, and when WS = 1, right channel is sampled.
