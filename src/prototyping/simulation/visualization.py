"""
Plotting helpers for simulated rooms.
"""

from __future__ import annotations

from typing import Optional

import matplotlib.pyplot as plt
from pydub import AudioSegment
from .database import save_audio_data, write_readme
import pyroomacoustics as pra
import numpy as np


def plot_room(room: pra.ShoeBox, x_lim: tuple = (0, 2000), y_lim: tuple = (0, 1500)) -> None:
    """
    Plot the room layout using matplotlib with the given x (frequency) and y (magnitude)
    limits for the microphone responses.

    :param room: The ShoeBox object representing the room.
    :param x_lim: The x limits of the plot for the frequency domain.
    :param y_lim: The y limits of the plot for the magnitude domain.
    """

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


def save_room_data(
    room: pra.ShoeBox,
    title: str,
    description: str,
    output_dir: Optional[str] = None,
) -> bool:
    """
    Save the room data to a local directory.
    
    :param room: The ShoeBox object representing the room.
    :param title: The title of the test case.
    :param description: The description of the test case.
    :param output_dir: Optional directory name override for where to save results.
    :return: Boolean value representing success of save operation.
    """

    # Default directory name derived from title when none is provided
    output_dir = output_dir or title.replace(" ", "_")

    fs_hz = room.fs
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
            frame_rate=fs_hz,
            sample_width=2,  # int16
            channels=1,
        )

        if (save_audio_data(audio, f"mic_{i+1}.mp3", dir_name=output_dir)):
            print(f"Saved mono MP3 files for microphone {i+1} to: {output_dir}")
        else:
            print(f"Failed to save mono MP3 files for microphone {i+1} to: {output_dir}")
            return False
    
    # Write the README.md file for the test case
    write_readme(output_dir, _format_readme(room, title, description))

    print(f"Saved mono MP3 files to: {output_dir}")
    return True


def _format_readme(room: pra.ShoeBox, title: str, description: str) -> str:
    """
    Format the README.md file for the test case.

    :param room: The ShoeBox object representing the room.
    :param title: The title of the test case.
    :param description: The description of the test case.
    :return: The formatted README.md file.
    """

    content = f"# {title}\n\n{description}\n\n"
    
    # Room dimensions
    content += f"## Room Dimensions\n\n"
    content += f"- X: {room.shoebox_dim[0]:.2f} m\n"
    content += f"- Y: {room.shoebox_dim[1]:.2f} m\n"
    content += f"- Z: {room.shoebox_dim[2]:.2f} m\n\n"
    
    # Sampling frequency
    content += f"## Sampling Frequency\n\n{room.fs} Hz\n\n"
    
    # Microphone positions
    content += f"## Microphone Positions\n\n"
    num_mics = room.mic_array.R.shape[1]
    for i in range(num_mics):
        pos = room.mic_array.R[:, i]
        content += f"- Microphone {i+1}: ({pos[0]:.2f}, {pos[1]:.2f}, {pos[2]:.2f})\n"
    content += "\n"
    
    # Source information
    content += f"## Sound Sources\n\n"
    content += f"Number of sources: {len(room.sources)}\n\n"
    for i, source in enumerate(room.sources):
        content += f"### Source {i+1}\n"
        content += f"- Position: ({source.position[0]:.2f}, {source.position[1]:.2f}, {source.position[2]:.2f})\n"
        if source.signal is not None:
            content += f"- Signal length: {len(source.signal)} samples ({len(source.signal)/room.fs:.3f} seconds)\n"
        else:
            content += f"- Signal: None\n"
        content += f"- Delay: {source.delay} seconds\n"
        content += "\n"
    
    return content

# Persistent state
_doa_fig = None
_doa_ax = None
_doa_est_arrow_artists = {}
_doa_est_text_artists = {}
_doa_true_arrow = None
_classification = None

def visualize_simulation_continuous(doa_results: dict,
                                mic_positions: np.ndarray,
                                classification,
                                true_source_position: Optional[np.ndarray] = None):
    """
    Real-time DOA visualization that updates arrow directions instead of redrawing the plot.
    """
    global _doa_fig, _doa_ax, _doa_est_arrow_artists, _doa_est_text_artists, _doa_true_arrow, _classification

    # ---- INITIAL SETUP (only once) ----
    if _doa_fig is None:
        plt.ion()
        _doa_fig, _doa_ax = plt.subplots(figsize=(8, 8))

        mic_center = np.mean(mic_positions[:2, :], axis=1)
        mic_x = mic_positions[0, :] - mic_center[0]
        mic_y = mic_positions[1, :] - mic_center[1]

        # Mic positions
        _doa_ax.scatter(mic_x, mic_y, c='blue', s=120, zorder=3)
        for i in range(mic_positions.shape[1]):
            _doa_ax.text(mic_x[i], mic_y[i], f"Mic {i+1}", fontsize=9, ha="center", va="center")

        _doa_ax.set_xlim(-1, 1)
        _doa_ax.set_ylim(-1, 1)
        _doa_ax.set_aspect("equal")
        _doa_ax.grid(True, alpha=0.3)
        _doa_ax.set_title("Real-Time DOA Estimation (0° = North)")

        _classification = _doa_ax.text(
            0.5, -1.15, 
            f"Classification: {classification}", 
            ha="center", va="top", fontsize=12, transform=_doa_ax.transData
        )

        # ---- TRUE SOURCE DIRECTION STATIC ARROW ----
        if true_source_position is not None:
            source_x = true_source_position[0] - mic_center[0]
            source_y = true_source_position[1] - mic_center[1]

            true_angle = np.arctan2(source_y, source_x)   # ✅ correct
            sx = 0.6 * np.cos(true_angle)
            sy = 0.6 * np.sin(true_angle)

            _doa_true_arrow = _doa_ax.arrow(
                0, 0, sx, sy,
                head_width=0.05, head_length=0.05,
                fc='black', ec='black', linestyle='--', linewidth=2, alpha=0.7
            )
            _doa_ax.text(sx*1.15, sy*1.15, "True Source", fontsize=10, color='black')

    # ---- CLEAR PREVIOUS ESTIMATED DOA ARROWS ----
    for artist in list(_doa_est_arrow_artists.values()) + list(_doa_est_text_artists.values()):
        artist.remove()

    _doa_est_arrow_artists.clear()
    _doa_est_text_artists.clear()

    # ---- PLOT UPDATED ESTIMATED DOA ----
    colors = ['red', 'green', 'orange', 'purple']

    for idx, (name, doa) in enumerate(doa_results.items()):
        if doa.azimuth_recon is None or len(doa.azimuth_recon) == 0:
            continue

        color = colors[idx % len(colors)]

        for src_i, azimuth_rad in enumerate(doa.azimuth_recon):
            azimuth_deg = np.rad2deg(azimuth_rad)

            # Convert degrees to unit direction
            x = 0.6 * np.cos(np.deg2rad(azimuth_deg))
            y = 0.6 * np.sin(np.deg2rad(azimuth_deg))

            arrow = _doa_ax.arrow(0, 0, x, y, fc=color, ec=color, width=0.02)
            text = _doa_ax.text(x*1.15, y*1.15, f"{name}: {azimuth_deg:.1f}°", fontsize=9, color=color)

            _doa_est_arrow_artists[f"{name}_{src_i}"] = arrow
            _doa_est_text_artists[f"{name}_{src_i}"] = text

    if _classification is not None:
        _classification.set_text(f"Classification: {classification}")
    plt.pause(0.01)