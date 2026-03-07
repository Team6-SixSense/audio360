#!/usr/bin/env python3
"""
Generate three_tone.wav audio file for FFT testing.
Contains 100 Hz, 1 kHz, and 7.5 kHz sine tones.
Duration: 3 seconds
Sample rate: 16000 Hz
"""

import numpy as np
import soundfile as sf

# Audio parameters
sample_rate = 16000
duration = 3.0  # seconds
frequencies = [100.0, 1000.0, 7500.0]  # Hz
amplitudes = [0.2, 0.2, 0.6]  # Relative amplitudes

# Generate time array
num_samples = int(sample_rate * duration)
t = np.linspace(0, duration, num_samples, endpoint=False)

# Generate composite signal
signal = np.zeros(num_samples, dtype=np.float32)
for freq, amp in zip(frequencies, amplitudes):
    signal += amp * np.sin(2 * np.pi * freq * t)

# Normalize to prevent clipping
max_amplitude = np.max(np.abs(signal))
if max_amplitude > 0:
    signal = signal / max_amplitude * 0.95  # Scale to 95% to leave headroom

# Output path
output_path = '../test/audio/three_tone.wav'

# Write WAV file
sf.write(output_path, signal, sample_rate, subtype='PCM_16')

print(f"✓ Generated {output_path}")
print(f"  Sample rate: {sample_rate} Hz")
print(f"  Duration: {duration} seconds")
print(f"  Frequencies: {frequencies} Hz")
print(f"  Amplitudes: {amplitudes}")
print(f"  Samples: {num_samples}")
