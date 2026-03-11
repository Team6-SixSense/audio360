"""
ICS43434 Diagnostic — Dump raw hex values from the USB mic stream.
This helps determine: is the data all zeros (wrong slot), random (signal
integrity), or structured but needing a different byte transform?

Run with: python scripts/wav/mic_diagnostic.py
"""

import serial
import struct
import sys
import time

# --- CONFIGURATION ---
SERIAL_PORT = 'COM7'
BAUD_RATE = 115200
CHANNELS = 4
INT32_SIZE = 4
FRAME_SIZE = INT32_SIZE * CHANNELS

# Setup Serial
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT}")
except serial.SerialException:
    print(f"Error: Could not open {SERIAL_PORT}.")
    sys.exit(1)

# Flush any stale data
time.sleep(0.5)
ser.reset_input_buffer()

print("\nReading raw mic data (4 channels interleaved)...")
print("Each row = one frame: [A1, B1, A2, B2] as raw hex uint32\n")
print(f"{'Frame':>6}  {'A1':>12}  {'B1':>12}  {'A2':>12}  {'B2':>12}  | {'A1 decimal':>14}")
print("-" * 85)

byte_buffer = bytearray()
frame_count = 0
MAX_FRAMES = 200

# Statistics
all_zero_count = 0
unique_values = set()

while frame_count < MAX_FRAMES:
    if ser.in_waiting > 0:
        byte_buffer.extend(ser.read(ser.in_waiting))
    else:
        time.sleep(0.001)
        continue

    while len(byte_buffer) >= FRAME_SIZE:
        frame_bytes = byte_buffer[:FRAME_SIZE]
        byte_buffer = byte_buffer[FRAME_SIZE:]

        # Unpack as 4 unsigned 32-bit integers (little-endian)
        values = struct.unpack('<4I', frame_bytes)

        # Also show the raw bytes for channel A1
        a1_bytes = frame_bytes[0:4]

        frame_count += 1

        if values[0] == 0:
            all_zero_count += 1

        unique_values.add(values[0])

        # Print first 50 frames, then every 10th
        if frame_count <= 50 or frame_count % 10 == 0:
            signed_a1 = struct.unpack('<i', frame_bytes[0:4])[0]
            print(
                f"{frame_count:>6}  "
                f"0x{values[0]:08X}  0x{values[1]:08X}  "
                f"0x{values[2]:08X}  0x{values[3]:08X}  | "
                f"{signed_a1:>14d}"
            )

ser.close()

print(f"\n{'='*85}")
print(f"Summary over {MAX_FRAMES} frames:")
print(f"  All-zero A1 samples: {all_zero_count}/{MAX_FRAMES}")
print(f"  Unique A1 values:    {len(unique_values)}")

if all_zero_count == MAX_FRAMES:
    print("\n  >> ALL ZEROS: Mic is not sending data on Slot 0 (left channel).")
    print("     Check L/R pin wiring, or try SlotActive = 0x00000002 for slot 1.")
elif all_zero_count > MAX_FRAMES * 0.9:
    print("\n  >> MOSTLY ZEROS with occasional values: Possible sync issue.")
elif len(unique_values) < 10:
    print("\n  >> Very few unique values: Possible stuck data or constant output.")
else:
    # Check if values look like they have a pattern
    print(f"\n  >> Data looks active ({len(unique_values)} unique values).")
    print("     Examine the hex values above to determine byte ordering.")
