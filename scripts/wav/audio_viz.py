import serial
import struct
import numpy as np
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import RadioButtons
import sys
import sounddevice as sd
import threading
import time

# --- CONFIGURATION ---
SERIAL_PORT = 'COM6'
BAUD_RATE = 115200
SAMPLE_RATE = 16000
WINDOW_DURATION = 0.2

# Volume Scaling
# Since we are doing manual math now, we can push this up.
VOLUME = 0.3

# We now have 4 channels interleaved: A1, B1, A2, B2
CHANNELS = 4
SELECTED_CHANNEL = 0  # 0: A1, 1: B1, 2: A2, 3: B2

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
fig, axes = plt.subplots(4, 1, figsize=(10, 10), sharex=True)
plt.subplots_adjust(bottom=0.2, hspace=0.3)
rax = plt.axes([0.4, 0.02, 0.2, 0.12])

x_data = np.arange(WINDOW_SAMPLES)
y_data_buffers = [np.zeros(WINDOW_SAMPLES, dtype=np.int32) for _ in range(CHANNELS)]
lines = []
colors = ['#00ff00', '#ff0000', '#0000ff', '#ffff00']
labels = ['A1', 'B1', 'A2', 'B2']

fig.patch.set_facecolor('#1e1e1e')

for i, ax in enumerate(axes):
    line, = ax.plot(x_data, y_data_buffers[i], color=colors[i], linewidth=1)
    lines.append(line)
    ax.set_facecolor('black')
    ax.set_ylim(-Y_LIMIT, Y_LIMIT)
    ax.set_xlim(0, WINDOW_SAMPLES)
    ax.set_title(f"Mic {labels[i]}", color='white', fontsize=10)
    ax.grid(True, color='gray', linestyle='--', alpha=0.3)
    if i < CHANNELS - 1:
        ax.set_xticklabels([])

byte_buffer = bytearray()

# Threading globals
running = True
data_lock = threading.Lock()


def select_channel_radio(label):
    global SELECTED_CHANNEL

    if label == 'A1':
        SELECTED_CHANNEL = 0
    elif label == 'A2':
        SELECTED_CHANNEL = 2
    elif label == 'B1':
        SELECTED_CHANNEL = 1
    elif label == 'B2':
        SELECTED_CHANNEL = 3

radio_button = RadioButtons(rax, ('A1', 'A2', 'B1', 'B2'))
radio_button.on_clicked(select_channel_radio)

def process_data_thread():
    global byte_buffer, y_data_buffers, running

    while running:
        try:
            if ser.in_waiting > 0:
                new_bytes = ser.read(ser.in_waiting)
                byte_buffer.extend(new_bytes)
            else:
                time.sleep(0.001)
        except OSError:
            pass
        except Exception as e:
            print(f"Error reading serial: {e}")
            time.sleep(0.1)
            continue

        # Ensure we have full frames of data (4 channels * 4 bytes)
        FRAME_SIZE = INT32_SIZE * CHANNELS

        if len(byte_buffer) >= FRAME_SIZE:
            num_frames = len(byte_buffer) // FRAME_SIZE
            bytes_to_process = num_frames * FRAME_SIZE
            chunk = byte_buffer[:bytes_to_process]
            byte_buffer = byte_buffer[bytes_to_process:]

            # 1. Read Raw 32-bit integers
            raw_samples = np.frombuffer(chunk, dtype=np.int32)

            # 2. Reshape to separate channels: [Samples, Channels]
            # Column 0 = A1, 1 = B1, 2 = A2, 3 = B2
            reshaped_samples = raw_samples.reshape(-1, CHANNELS)

            for i in range(CHANNELS):
                channel_data = reshaped_samples[:, i]

                # 2. Convert to Float immediately for safe math
                samples_float = channel_data.astype(np.float32)

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

                if i == SELECTED_CHANNEL:
                    try:
                        stream.write(final_samples)
                    except Exception:
                        pass

                with data_lock:
                    shift = len(final_samples)
                    if shift >= WINDOW_SAMPLES:
                        y_data_buffers[i][:] = final_samples[-WINDOW_SAMPLES:]
                    else:
                        y_data_buffers[i] = np.roll(y_data_buffers[i], -shift)
                        y_data_buffers[i][-shift:] = final_samples

def update_plot(frame):
    with data_lock:
        for i in range(CHANNELS):
            lines[i].set_ydata(y_data_buffers[i])
    return lines

# Start processing thread
t = threading.Thread(target=process_data_thread)
t.daemon = True
t.start()

ani = FuncAnimation(fig, update_plot, interval=60, blit=False, cache_frame_data=False)
plt.show()

running = False
t.join()
stream.stop()
stream.close()
ser.close()
