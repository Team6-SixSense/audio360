import wave
import os
import sys
import struct  # NEW: Import the struct module

# --- Please Configure These Values ---

# The name of your raw binary file from the SD card
INPUT_FILE = 'AUDIO.TXT'  # <-- CHANGE THIS

# The name of the .wav file you want to create
OUTPUT_FILE = 'output_amplified.wav'

# Your recording parameters (must match the STM32)
SAMPLE_RATE = 16000
NUM_CHANNELS = 1
SAMPLE_WIDTH_BYTES = 4  # 4 bytes for 32-bit signed audio (s32le)

# --- End of Configuration ---


# NEW: Define the format for one 32-bit signed little-endian sample
STRUCT_FORMAT = '<l'

print(f"Converting and amplifying '{INPUT_FILE}'...")

try:
    # Get the file size to calculate the number of frames
    file_size = os.path.getsize(INPUT_FILE)
    if file_size == 0:
        print(f"Error: Input file '{INPUT_FILE}' is empty.")
        sys.exit(1)

    bytes_per_frame = NUM_CHANNELS * SAMPLE_WIDTH_BYTES
    if file_size % bytes_per_frame != 0:
        print(f"Warning: File size ({file_size} bytes) is not a perfect multiple"
              f" of the frame size ({bytes_per_frame} bytes).")

    num_frames = file_size // bytes_per_frame
    duration_sec = num_frames / SAMPLE_RATE

    print(f"  Sample Rate: {SAMPLE_RATE} Hz")
    print(f"  Channels: {NUM_CHANNELS}")
    print(f"  Sample Width: {SAMPLE_WIDTH_BYTES} bytes (32-bit)")
    print(f"  Total Frames: {num_frames}")
    print(f"  Duration: {duration_sec:.2f} seconds")


    # NEW: Create a byte array to hold the new, amplified data
    amplified_data = bytearray()

    # Open the raw binary file to read
    with open(INPUT_FILE, 'rb') as raw_file:
        while True:
            # Read one sample's worth of bytes
            sample_bytes = raw_file.read(SAMPLE_WIDTH_BYTES)

            # Stop when we reach the end of the file
            if not sample_bytes:
                break

            # Make sure we read a full sample
            if len(sample_bytes) < SAMPLE_WIDTH_BYTES:
                print("Warning: Incomplete sample at end of file. Skipping.")
                continue

            # 1. Unpack the 4 bytes into a single integer
            sample_int, = struct.unpack(STRUCT_FORMAT, sample_bytes)

            # 2. Amplify it by bit-shifting left 8 bits
            amplified_sample_int = sample_int << 8

            try:
                # 3. Pack the new, louder integer back into 4 bytes
                amplified_sample_bytes = struct.pack(STRUCT_FORMAT, amplified_sample_int)
            except struct.error:
                # This handles a potential overflow if the data wasn't 24-bit
                print(f"Error: Value {amplified_sample_int} overflowed 32-bit size.")
                print("Clipping to max/min value.")
                if amplified_sample_int > 0:
                    amplified_sample_int = 2147483647  # Max 32-bit positive
                else:
                    amplified_sample_int = -2147483648 # Max 32-bit negative
                amplified_sample_bytes = struct.pack(STRUCT_FORMAT, amplified_sample_int)

            # 4. Add the new bytes to our final data
            amplified_data.extend(amplified_sample_bytes)


    # Open the output .wav file to write
    with wave.open(OUTPUT_FILE, 'wb') as wav_file:
        # Set the WAV file parameters
        wav_file.setnchannels(NUM_CHANNELS)
        wav_file.setsampwidth(SAMPLE_WIDTH_BYTES)
        wav_file.setframerate(SAMPLE_RATE)

        # Write the new, amplified audio data
        wav_file.writeframes(amplified_data)

    print(f"\nSuccess! File saved as: {OUTPUT_FILE}")

except FileNotFoundError:
    print(f"Error: Input file '{INPUT_FILE}' not found.")
except Exception as e:
    print(f"An unexpected error occurred: {e}")