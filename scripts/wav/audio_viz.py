import serial
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
import sys
import sounddevice as sd

# --- CONFIGURATION ---
SERIAL_PORT = 'COM5'
BAUD_RATE = 115200
SAMPLE_RATE = 16000
WINDOW_DURATION = 0.2

# Volume Scaling
# Since we are doing manual math now, we can push this up.
VOLUME = 0.3

STEREO_ON_SOURCE = False

# Constants
INT32_SIZE = 4
WINDOW_SAMPLES = int(SAMPLE_RATE * WINDOW_DURATION)
Y_LIMIT = 2 ** 31

# Setup Serial
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=0)
    print(f"Connected to {SERIAL_PORT}")
except serial.SerialException:
    print(f"Error: Could not open {SERIAL_PORT}.")
    sys.exit(1)

# Setup Audio
try:
    stream = sd.OutputStream(samplerate=SAMPLE_RATE, channels=1, dtype='int32', latency='low')
    stream.start()
    print("Audio stream started.")
except Exception as e:
    print(f"Audio error: {e}")
    sys.exit(1)

# --- PLOTTING ---

# Setup Plot
fig, ax = plt.subplots(figsize=(10, 4))
x_data = np.arange(WINDOW_SAMPLES)
y_data = np.zeros(WINDOW_SAMPLES, dtype=np.int32)
line, = ax.plot(x_data, y_data, color='#00ff00', linewidth=1)

ax.set_facecolor('black')
fig.patch.set_facecolor('#1e1e1e')
ax.set_ylim(-Y_LIMIT, Y_LIMIT)
ax.set_xlim(0, WINDOW_SAMPLES)
ax.set_title(f"Live Stream (Vol: {VOLUME})", color='white')
ax.grid(True, color='gray', linestyle='--', alpha=0.3)

byte_buffer = bytearray()


def update_plot(frame):
    """
    @brief Updates the plot based on received audio data over COM port.
    @param frame Unused parameter.
    """

    global y_data, byte_buffer

    try:
        if ser.in_waiting > 0:
            new_bytes = ser.read(ser.in_waiting)
            byte_buffer.extend(new_bytes)
    except OSError:
        return line,

    if len(byte_buffer) >= INT32_SIZE:
        num_samples = len(byte_buffer) // INT32_SIZE
        bytes_to_process = num_samples * INT32_SIZE
        chunk = byte_buffer[:bytes_to_process]
        byte_buffer = byte_buffer[bytes_to_process:]

        # 1. Read Raw 32-bit integers
        raw_samples = np.frombuffer(chunk, dtype=np.int32)

        if STEREO_ON_SOURCE:
            raw_samples = raw_samples[::2]

        # 2. Convert to Float immediately for safe math
        samples_float = raw_samples.astype(np.float32)

        # 3. DC Offset Removal (Safe now because we are in Float)
        dc_offset = np.mean(samples_float)
        samples_centered = samples_float - dc_offset

        # 4. Apply Gain (x256 to simulate the 8-bit shift)
        # We scale by 256 to restore the volume, then apply your VOLUME knob.
        samples_scaled = samples_centered * 256.0 * VOLUME

        # 5. Hard Clip Limiter (Prevent wrapping noise)
        # We clamp values to the 32-bit valid range so they simply "flat top"
        # instead of wrapping around to static.
        np.clip(samples_scaled, -2147483647, 2147483647, out=samples_scaled)

        # 6. Convert back to Int32 for the speakers
        final_samples = samples_scaled.astype(np.int32)

        try:
            stream.write(final_samples)
        except Exception:
            pass

        shift = len(final_samples)
        if shift >= WINDOW_SAMPLES:
            y_data[:] = final_samples[-WINDOW_SAMPLES:]
        else:
            y_data = np.roll(y_data, -shift)
            y_data[-shift:] = final_samples
        line.set_ydata(y_data)

    return line,


ani = FuncAnimation(fig, update_plot, interval=1, blit=True, cache_frame_data=False)
plt.show()

stream.stop()
stream.close()
ser.close()
