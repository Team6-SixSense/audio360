"""
This module provides Direction of Arrival (DOA) analysis functionality for multi-microphone
audio recordings. It processes pre-recorded audio from 4 microphones and estimates the 
direction of the sound source using various DOA algorithms.
"""

import numpy as np
import matplotlib.pyplot as plt
from pydub import AudioSegment
import pyroomacoustics as pra
from database_helper import load_audio_data
from typing import List, Tuple, Optional


def load_microphone_signals(dir_name: str, num_mics: int = 4) -> Tuple[np.ndarray, int]:
    """
    Load audio signals from multiple microphone MP3 files.
    
    :param dir_name: The directory name containing the microphone MP3 files.
    :param num_mics: The number of microphones to load (default: 4).
    :return: A tuple containing:
        - signals: numpy array of shape (num_mics, num_samples) with audio data
        - fs: sampling frequency in Hz
    """
    
    signals = []
    fs = None
    
    for i in range(1, num_mics + 1):
        filename = f"mic_{i}.mp3"
        print(f"Loading {filename} from {dir_name}...")
        
        audio = load_audio_data(dir_name, filename)
        if audio is None:
            raise FileNotFoundError(f"Could not load {filename} from {dir_name}")
        
        # Get sampling frequency (only need to do this once)
        if fs is None:
            fs = audio.frame_rate
        
        # Convert to numpy array
        samples = np.array(audio.get_array_of_samples()).astype(np.float64)
        
        # Normalize to [-1, 1] range
        samples = samples / np.max(np.abs(samples))
        
        signals.append(samples)
    
    # Stack into (num_mics, num_samples) array
    signals = np.array(signals)
    
    print(f"Loaded {num_mics} microphone signals with {signals.shape[1]} samples at {fs} Hz")
    return signals, fs


def calculate_nfft(signal_length: int) -> int:
    """
    Calculate an appropriate NFFT size based on signal length.
    
    NFFT should be:
    - A power of 2 for FFT efficiency
    - Smaller than or equal to signal length
    - Large enough to provide good frequency resolution (at least 128)
    - Not too large (pyroomacoustics DOA works best with NFFT <= 1024)
    
    :param signal_length: length of the signal in samples
    :return: appropriate NFFT size
    """
    
    # For DOA algorithms, use conservative NFFT values
    # Pyroomacoustics DOA algorithms work best with smaller NFFT
    nfft_options = [128, 256, 512, 1024]
    
    # Find the largest NFFT that's smaller than signal length
    for nfft in reversed(nfft_options):
        if nfft <= signal_length:
            return nfft
    
    # If signal is very short, use the next power of 2 below signal length
    nfft = 1
    while nfft * 2 <= signal_length and nfft < 1024:
        nfft *= 2
    
    # Ensure minimum NFFT of 64, maximum of 1024
    return max(min(nfft, 1024), 64)


def convert_to_stft(signals: np.ndarray, nfft: int, hop: int = None) -> np.ndarray:
    """
    Convert time-domain signals to STFT (Short-Time Fourier Transform) format.
    
    Pyroomacoustics DOA algorithms expect frequency-domain data with shape:
    (n_mics, n_freq_bins, n_snapshots) where n_freq_bins = nfft//2 + 1
    
    :param signals: numpy array of shape (n_mics, n_samples) with time-domain signals
    :param nfft: FFT size
    :param hop: hop size between windows (default: nfft//2)
    :return: STFT array of shape (n_mics, nfft//2+1, n_snapshots)
    """
    
    if hop is None:
        hop = nfft // 2
    
    n_mics, n_samples = signals.shape
    n_freq_bins = nfft // 2 + 1
    n_snapshots = (n_samples - nfft) // hop + 1
    
    # Initialize STFT array
    X = np.zeros((n_mics, n_freq_bins, n_snapshots), dtype=np.complex128)
    
    # Compute STFT for each microphone
    for mic_idx in range(n_mics):
        for snap_idx in range(n_snapshots):
            start = snap_idx * hop
            end = start + nfft
            
            # Extract window and apply Hanning window
            window = signals[mic_idx, start:end] * np.hanning(nfft)
            
            # Compute RFFT (real FFT, only positive frequencies)
            X[mic_idx, :, snap_idx] = np.fft.rfft(window, n=nfft)
    
    return X


def setup_microphone_array(mic_positions: np.ndarray, fs: int) -> pra.MicrophoneArray:
    """
    Set up a microphone array object for DOA processing.
    
    :param mic_positions: numpy array of shape (3, num_mics) with [x, y, z] coordinates 
                          for each microphone
    :param fs: sampling frequency in Hz
    :return: MicrophoneArray object configured for DOA
    """
    
    return pra.MicrophoneArray(mic_positions, fs)


def estimate_doa_music(signals: np.ndarray, 
                       mic_array: pra.MicrophoneArray,
                       num_sources: int = 1,
                       azimuth_resolution: int = 360) -> pra.doa.MUSIC:
    """
    Estimate Direction of Arrival using the MUSIC (Multiple Signal Classification) algorithm.
    
    :param signals: numpy array of shape (num_mics, num_samples) with microphone signals
    :param mic_array: MicrophoneArray object with microphone positions
    :param num_sources: expected number of sound sources (default: 1)
    :param azimuth_resolution: number of azimuth angles to search (default: 360)
    :return: MUSIC DOA object with estimated directions
    """
    
    print("Running MUSIC algorithm...")
    
    # Use a fixed, known-good NFFT value
    nfft = 256
    print(f"  Using NFFT = {nfft}")
    
    # Convert time-domain signals to STFT (frequency domain)
    # pyroomacoustics DOA expects shape (n_mics, n_freq_bins, n_snapshots)
    X = convert_to_stft(signals, nfft)
    print(f"  STFT shape: {X.shape} (mics x freq_bins x snapshots)")
    
    # Create MUSIC DOA object
    doa_music = pra.doa.MUSIC(mic_array.R, fs=mic_array.fs, nfft=nfft, num_src=num_sources)
    
    # Use frequency range that's guaranteed to be valid
    # For 8kHz sampling: max frequency is 4kHz (Nyquist), use safe range
    max_freq = min(mic_array.fs // 2 - 500, 3000)
    min_freq = 300
    freq_range = [min_freq, max_freq]
    print(f"  Frequency range: {freq_range[0]}-{freq_range[1]} Hz")
    
    # Run DOA estimation with STFT data
    doa_music.locate_sources(X, freq_range=freq_range)
    
    return doa_music


def estimate_doa_srp(signals: np.ndarray,
                     mic_array: pra.MicrophoneArray,
                     num_sources: int = 1) -> pra.doa.SRP:
    """
    Estimate Direction of Arrival using the SRP-PHAT (Steered Response Power with 
    Phase Transform) algorithm.
    
    :param signals: numpy array of shape (num_mics, num_samples) with microphone signals
    :param mic_array: MicrophoneArray object with microphone positions
    :param num_sources: expected number of sound sources (default: 1)
    :return: SRP DOA object with estimated directions
    """
    
    print("Running SRP-PHAT algorithm...")
    
    # Use a fixed, known-good NFFT value
    nfft = 256
    print(f"  Using NFFT = {nfft}")
    
    # Convert time-domain signals to STFT (frequency domain)
    X = convert_to_stft(signals, nfft)
    print(f"  STFT shape: {X.shape} (mics x freq_bins x snapshots)")
    
    # Create SRP-PHAT DOA object
    doa_srp = pra.doa.SRP(mic_array.R, fs=mic_array.fs, nfft=nfft, num_src=num_sources)
    
    # Use frequency range that's guaranteed to be valid
    max_freq = min(mic_array.fs // 2 - 500, 3000)
    min_freq = 300
    freq_range = [min_freq, max_freq]
    print(f"  Frequency range: {freq_range[0]}-{freq_range[1]} Hz")
    
    # Run DOA estimation with STFT data
    doa_srp.locate_sources(X, freq_range=freq_range)
    
    return doa_srp


def estimate_doa_gcc_phat(signals: np.ndarray,
                          mic_array: pra.MicrophoneArray,
                          num_sources: int = 1) -> pra.doa.FRIDA:
    """
    Estimate Direction of Arrival using the FRIDA (FRI-based DOA) algorithm with GCC-PHAT.
    
    :param signals: numpy array of shape (num_mics, num_samples) with microphone signals
    :param mic_array: MicrophoneArray object with microphone positions
    :param num_sources: expected number of sound sources (default: 1)
    :return: FRIDA DOA object with estimated directions
    """
    
    print("Running FRIDA algorithm...")
    
    # Use a fixed, known-good NFFT value
    nfft = 256
    print(f"  Using NFFT = {nfft}")
    
    # Convert time-domain signals to STFT (frequency domain)
    X = convert_to_stft(signals, nfft)
    print(f"  STFT shape: {X.shape} (mics x freq_bins x snapshots)")
    
    # Create FRIDA DOA object
    doa_frida = pra.doa.FRIDA(mic_array.R, fs=mic_array.fs, nfft=nfft, num_src=num_sources)
    
    # Use frequency range that's guaranteed to be valid
    max_freq = min(mic_array.fs // 2 - 500, 3000)
    min_freq = 300
    freq_range = [min_freq, max_freq]
    print(f"  Frequency range: {freq_range[0]}-{freq_range[1]} Hz")
    
    # Run DOA estimation with STFT data
    doa_frida.locate_sources(X, freq_range=freq_range)
    
    return doa_frida


def angles_to_cartesian(azimuth_deg: float, radius: float = 1.0) -> Tuple[float, float]:
    """
    Convert azimuth angle in degrees to 2D Cartesian coordinates.
    
    :param azimuth_deg: azimuth angle in degrees (0° = North, 90° = East)
    :param radius: radius from origin (default: 1.0)
    :return: tuple of (x, y) Cartesian coordinates
    """
    
    azimuth_rad = np.deg2rad(azimuth_deg)
    x = radius * np.sin(azimuth_rad)
    y = radius * np.cos(azimuth_rad)
    return x, y


def visualize_doa_2d(doa_results: dict,
                     mic_positions: np.ndarray,
                     true_source_position: Optional[np.ndarray] = None,
                     save_path: Optional[str] = None) -> None:
    """
    Visualize DOA estimation results on a 2D plane with microphone positions.
    
    :param doa_results: dictionary with algorithm names as keys and DOA objects as values
    :param mic_positions: numpy array of shape (3, num_mics) with microphone coordinates
    :param true_source_position: optional numpy array with true source position [x, y, z]
    :param save_path: optional path to save the figure
    """
    
    fig, ax = plt.subplots(1, 1, figsize=(10, 10))
    
    # Plot microphone positions (use x,y coordinates from 3D positions)
    mic_center = np.mean(mic_positions[:2, :], axis=1)
    mic_x = mic_positions[0, :] - mic_center[0]
    mic_y = mic_positions[1, :] - mic_center[1]
    
    ax.scatter(mic_x, mic_y, c='blue', s=100, marker='o', label='Microphones', zorder=3)
    
    # Label each microphone
    for i in range(mic_positions.shape[1]):
        ax.annotate(f'Mic {i+1}', 
                   (mic_x[i], mic_y[i]), 
                   xytext=(5, 5), 
                   textcoords='offset points',
                   fontsize=8)
    
    # Plot estimated DOA directions
    colors = ['red', 'green', 'orange', 'purple']
    for idx, (name, doa) in enumerate(doa_results.items()):
        if doa.azimuth_recon is None or len(doa.azimuth_recon) == 0:
            print(f"Warning: No DOA estimates found for {name}")
            continue
            
        color = colors[idx % len(colors)]
        
        for source_idx, azimuth_deg in enumerate(np.rad2deg(doa.azimuth_recon)):
            # Convert to cartesian (plot from origin)
            x, y = angles_to_cartesian(azimuth_deg, radius=0.5)
            
            # Plot arrow showing direction
            ax.arrow(0, 0, x, y, 
                    head_width=0.05, 
                    head_length=0.05, 
                    fc=color, 
                    ec=color,
                    alpha=0.7,
                    label=f'{name}: {azimuth_deg:.1f}°' if source_idx == 0 else None,
                    linewidth=2)
            
            # Add angle text at the end of arrow
            text_x, text_y = angles_to_cartesian(azimuth_deg, radius=0.6)
            ax.text(text_x, text_y, f'{azimuth_deg:.1f}°', 
                   ha='center', va='center',
                   fontsize=9,
                   bbox=dict(boxstyle='round,pad=0.3', facecolor=color, alpha=0.3))
    
    # Plot true source position if provided
    if true_source_position is not None:
        source_x = true_source_position[0] - mic_center[0]
        source_y = true_source_position[1] - mic_center[1]
        
        # Calculate true azimuth
        true_azimuth = np.arctan2(source_x, source_y)
        true_azimuth_deg = np.rad2deg(true_azimuth)
        
        # Normalize direction
        source_dist = np.sqrt(source_x**2 + source_y**2)
        source_x_norm = source_x / source_dist * 0.5
        source_y_norm = source_y / source_dist * 0.5
        
        ax.arrow(0, 0, source_x_norm, source_y_norm,
                head_width=0.05,
                head_length=0.05,
                fc='black',
                ec='black',
                linestyle='--',
                linewidth=2,
                alpha=0.5,
                label=f'True: {true_azimuth_deg:.1f}°')
    
    # Set plot properties
    ax.set_xlim(-0.8, 0.8)
    ax.set_ylim(-0.8, 0.8)
    ax.set_aspect('equal')
    ax.grid(True, alpha=0.3)
    ax.set_xlabel('X Position (m)', fontsize=12)
    ax.set_ylabel('Y Position (m)', fontsize=12)
    ax.set_title('Direction of Arrival (DOA) Estimation\n(0° = North, 90° = East)', 
                fontsize=14, fontweight='bold')
    ax.legend(loc='upper right', fontsize=10)
    
    # Add compass rose
    ax.annotate('N', xy=(0, 0.75), fontsize=12, ha='center', fontweight='bold')
    ax.annotate('E', xy=(0.75, 0), fontsize=12, ha='center', fontweight='bold')
    ax.annotate('S', xy=(0, -0.75), fontsize=12, ha='center', fontweight='bold')
    ax.annotate('W', xy=(-0.75, 0), fontsize=12, ha='center', fontweight='bold')
    
    plt.tight_layout()
    
    if save_path:
        plt.savefig(save_path, dpi=300, bbox_inches='tight')
        print(f"Saved DOA visualization to {save_path}")
    
    plt.show()


def analyze_doa(dir_name: str,
                mic_positions: Optional[np.ndarray] = None,
                true_source_position: Optional[np.ndarray] = None,
                algorithms: Optional[List[str]] = None) -> dict:
    """
    Main function to perform complete DOA analysis on recorded microphone signals.
    
    :param dir_name: directory containing mic_1.mp3, mic_2.mp3, mic_3.mp3, mic_4.mp3
    :param mic_positions: optional numpy array of shape (3, 4) with microphone positions.
                         If None, uses default square array positions.
    :param true_source_position: optional numpy array with true source position [x, y, z]
    :param algorithms: list of algorithms to use ['music', 'srp', 'frida']. 
                      If None, uses all available.
    :return: dictionary with algorithm names as keys and DOA objects as values
    """
    # Load microphone signals
    signals, fs = load_microphone_signals(dir_name)
    
    # Set default microphone positions if not provided (square array, 10cm spacing)
    if mic_positions is None:
        # Default positions based on example2.py
        mic_positions = np.array([
            [4.95, 5.05, 4.95, 5.05],  # x coordinates
            [5.10, 5.10, 5.00, 5.00],  # y coordinates
            [1.75, 1.75, 1.75, 1.75]   # z coordinates
        ])
        print("Using default microphone positions (square array)")
    
    print(f"Microphone positions:\n{mic_positions}\n")
    
    # Create microphone array
    mic_array = setup_microphone_array(mic_positions, fs)
    
    # Set default algorithms if not provided
    if algorithms is None:
        algorithms = ['music', 'srp', 'frida']
    
    # Run DOA estimation algorithms
    doa_results = {}
    
    if 'music' in algorithms:
        try:
            doa_results['MUSIC'] = estimate_doa_music(signals, mic_array)
            print(f"MUSIC estimated azimuth: {np.rad2deg(doa_results['MUSIC'].azimuth_recon)} degrees\n")
        except Exception as e:
            print(f"Error running MUSIC: {e}\n")
    
    if 'srp' in algorithms:
        try:
            doa_results['SRP-PHAT'] = estimate_doa_srp(signals, mic_array)
            print(f"SRP-PHAT estimated azimuth: {np.rad2deg(doa_results['SRP-PHAT'].azimuth_recon)} degrees\n")
        except Exception as e:
            print(f"Error running SRP-PHAT: {e}\n")
    
    if 'frida' in algorithms:
        try:
            doa_results['FRIDA'] = estimate_doa_gcc_phat(signals, mic_array)
            print(f"FRIDA estimated azimuth: {np.rad2deg(doa_results['FRIDA'].azimuth_recon)} degrees\n")
        except Exception as e:
            print(f"Error running FRIDA: {e}\n")
    
    # Visualize results
    print("Generating visualization...")
    visualize_doa_2d(doa_results, mic_positions, true_source_position)
    
    print("\n" + "="*70)
    print("DOA Analysis Complete")
    print("="*70 + "\n")
    
    return doa_results


def main():
    """
    Example usage of the DOA analysis module.
    """
    
    # Example: Analyze DOA from previously generated microphone recordings
    dir_name = "example_mp3_audio_sources"  # Change this to your data directory
    
    # Optional: specify true source position if known (from simulation)
    true_source = np.array([9.0, 9.0, 1.0])  # Example from example2.py
    
    # Run DOA analysis
    results = analyze_doa(
        dir_name=dir_name,
        true_source_position=true_source,
        algorithms=['music', 'srp']  # Can specify which algorithms to use
    )
    
    # Results can be accessed for further processing
    for algo_name, doa_obj in results.items():
        print(f"{algo_name} azimuth estimates: {np.rad2deg(doa_obj.azimuth_recon)} degrees")


if __name__ == "__main__":
    main()

