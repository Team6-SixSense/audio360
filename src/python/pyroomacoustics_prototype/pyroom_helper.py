"""
This module inclues all the helper functions to interact with the pyroomacoustics module.
"""

from datetime import datetime
import matplotlib.pyplot as plt
from pydub import AudioSegment
import pyroomacoustics as pra
import numpy as np

from database_helper import save_audio_data, load_audio_data


def create_room(room_dim_x: float, room_dim_y: float, room_dim_z: float, fs: int) -> pra.ShoeBox:
    """
    Create a room with the given dimensions and sampling frequency.

    :param room_dim_x: The x dimension of the room.
    :param room_dim_y: The y dimension of the room.
    :param room_dim_z: The z dimension of the room.
    :param fs: The sampling frequency of the room.
    :return: A ShoeBox object representing the room.
    """

    return pra.ShoeBox([room_dim_x, room_dim_y, room_dim_z], fs=fs)


def add_microphone(room: pra.ShoeBox, microphone_position: list) -> pra.ShoeBox:
    """
    Add a microphone to the room at the given position.

    :param room: The ShoeBox object representing the room.
    :param microphone_position: The position of the microphone as a list of [x, y, z] coordinates.
    :return: The ShoeBox object representing the room.
    """

    room.add_microphone(microphone_position)
    return room


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


def add_source_with_signal(room: pra.ShoeBox, source_position: list, signal: np.ndarray) -> pra.ShoeBox:
    """
    Add a source to the room with the given position and signal.

    :param room: The ShoeBox object representing the room.
    :param source_position: The position of the source as a list of [x, y, z] coordinates.
    :param signal: The signal to be added to the source.
    :return: The ShoeBox object representing the room.
    """

    room.add_source(source_position, signal=signal)
    return room


def plot_room(room: pra.ShoeBox, x_lim: tuple = (0, 2000), y_lim: tuple = (0, 1500)) -> None:
    """
    Plot the room layout using matplotlib with the given x (frequency) and y (magnitude)
    limits for the microphone responses.

    :param room: The ShoeBox object representing the room.
    :param x_lim: The x limits of the plot for the frequency domain.
    :param y_lim: The y limits of the plot for the magnitude domain.
    """

    print("Plotting room layout")

    print("Plotting room layout")
    room.plot()
    plt.title("Room Layout with Microphones and Source")
    plt.show()

    print("Plotting microphone responses")
    # Plot the recorded signals at each microphone
    fig, axes = plt.subplots(2, 2, figsize=(12, 8))
    axes = axes.flatten()
    num_channels = room.mic_array.R.shape[1]
    
    for i in range(num_channels):
        mic_signal = room.mic_array.signals[i, :]
        mic_pos = room.mic_array.R[:, i]
        time = np.linspace(0, len(mic_signal)/room.fs, len(mic_signal))
        
        axes[i].plot(time, mic_signal)
        axes[i].set_title(f'Microphone {i+1} Response at ({mic_pos[0]:.2f}, {mic_pos[1]:.2f})')
        axes[i].set_xlabel('Time (s)')
        axes[i].set_ylabel('Amplitude')
        axes[i].grid(True)
    
    plt.tight_layout()
    plt.show()
    
    print("Plotting frequency domain responses")
    # Plot frequency domain
    fig, axes = plt.subplots(2, 2, figsize=(12, 8))
    axes = axes.flatten()
    
    for i in range(num_channels):
        mic_signal = room.mic_array.signals[i, :]
        mic_pos = room.mic_array.R[:, i]
        # Compute FFT
        fft = np.fft.fft(mic_signal)
        freqs = np.fft.fftfreq(len(mic_signal), 1/room.fs)
        
        # Plot only positive frequencies
        positive_freqs = freqs[:len(freqs)//2]
        magnitude = np.abs(fft[:len(fft)//2])

        axes[i].plot(positive_freqs, magnitude)
        axes[i].set_title(f"Microphone {i+1} FFT at ({mic_pos[0]:.2f}, {mic_pos[1]:.2f})")
        axes[i].set_xlabel('Frequency (Hz)')
        axes[i].set_ylabel('Magnitude')
        axes[i].set_xlim(x_lim) # These values should be changeable
        axes[i].set_ylim(y_lim) 
        axes[i].grid(True)
    
    plt.tight_layout()
    plt.show()


def save_room_data(room: pra.ShoeBox) -> bool:
    """
    Save the room data to a local directory.

    :param room: The ShoeBox object representing the room.
    :return: Boolean value representing success of save operation.
    """

    # Get output directory from current timestamp
    output_dir = datetime.now().strftime("%Y%m%d_%H%M%S")

    fs = room.fs
    signals = room.mic_array.signals  # shape: (n_mics, n_samples)
    print(signals)

    # Normalize helper -> int16
    def to_int16(x: np.ndarray) -> np.ndarray:
        peak = float(np.max(np.abs(x))) if x.size > 0 else 1.0
        if peak == 0:
            peak = 1.0
        y = (x / peak * 32767.0).astype(np.int16)
        return y

    for i in range(signals.shape[0]):
        sig = to_int16(signals[i, :])
        audio = AudioSegment(
            sig.tobytes(),
            frame_rate=fs,
            sample_width=2,  # int16
            channels=1,
        )

        if (save_audio_data(audio, f"mic_{i+1}.mp3", dir_name=output_dir)):
            print(f"Saved mono MP3 files for microphone {i+1} to: {output_dir}")
        else:
            print(f"Failed to save mono MP3 files for microphone {i+1} to: {output_dir}")
            return False
    
    print(f"Saved mono MP3 files to: {output_dir}")
    return True
