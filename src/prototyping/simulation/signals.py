"""
Signal generation helpers.
"""

from __future__ import annotations

import numpy as np

from .database import load_audio_data


def generate_sine_wave(frequency: float, duration: float, fs: int, amplitude: float = 0.5) -> np.ndarray:
    """
    Generate a sine wave signal with the given frequency, duration, and sampling frequency.

    :param frequency: The frequency of the sine wave.
    :param duration: The duration of the sine wave.
    :param fs: The sampling frequency of the sine wave.
    :param amplitude: The amplitude of the sine wave.
    :return: A numpy array representing the sine wave signal.
    """

    t = np.linspace(0, duration, int(fs * duration), False)
    return amplitude * np.sin(2 * np.pi * frequency * t)


def generate_signal_from_audio_file(dirname: str, filename: str) -> np.ndarray:
    """
    Generate a signal from an audio file.

    :param dirname: The directory name of the audio file.
    :param filename: The filename of the audio file.
    :return: A numpy array representing the audio signal.
    """

    samples = load_audio_data(dirname, filename).get_array_of_samples()
    return np.array(samples)


def capture_audio_from_source(source_audio, rirs, mic_buffers, chunk_length):
    for m, rir in enumerate(rirs):
        # This stores what microphone m picks up from the source_chunk
        conv = np.convolve(source_audio, rir[0])[:chunk_length]
        
        # Add the audio stream to the buffer (automatically removes the stream from (BUFFER_SECONDS - 1) * CHUNK)
        mic_buffers[m].extend(conv)


    return mic_buffers