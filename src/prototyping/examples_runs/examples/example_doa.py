"""
This file provides sample usage for Direction of Arrival (DOA) analysis on pre-recorded
microphone data. It demonstrates how to process 4 microphone MP3 files and estimate
the direction of the audio source.
"""

from pathlib import Path
from simulation import analyze_doa
from simulation import classify_audio
import numpy as np


def main():
    """
    Example usage for DOA analysis on recorded microphone signals.
    
    This example analyzes the microphone recordings from example2.py (or any other
    simulation) to estimate the direction of arrival of the audio source.
    """
    
    ROOT = Path(__file__).resolve().parents[3]   # adjust depending on depth
    data_dir = ROOT / "prototyping/examples_runs/data/example_mp3_audio_sources_car"

    sound_dir = ROOT / "prototyping/examples_runs/data/example_mp3_audio_sources_car/mic_1.mp3"

    [pred, prob] = classify_audio(sound_dir)

    print(f"Prediction {pred}")
    print(f"Probability {prob}")
    # Note that this can be parsed from the README.md file if necessary 
    mic_positions = np.array([
        [4.95, 5.05, 4.95, 5.05],  # x coordinates (meters)
        [5.10, 5.10, 5.00, 5.00],  # y coordinates (meters)
        [1.75, 1.75, 1.75, 1.75]   # z coordinates (meters)
    ])
    true_source_pos = np.array([9.0, 9.0, 1.0])
    
    # Run DOA analysis with all available algorithms
    print("\nRunning DOA analysis with all algorithms...")
    results = analyze_doa(
        dir_name=data_dir,
        mic_positions=mic_positions,
        true_source_position=true_source_pos,
        algorithms=['music', 'srp', 'frida']  # Use all algorithms
    )
    

    # Print summary of results
    print("\n\n\nResults:")
    for algo_name, doa_obj in results.items():
        if doa_obj.azimuth_recon is not None and len(doa_obj.azimuth_recon) > 0:
            azimuth_deg = np.rad2deg(doa_obj.azimuth_recon)
            print(f"{algo_name:12s}: {azimuth_deg[0]:>6.2f}° (azimuth)")
        else:
            print(f"{algo_name:12s}: No estimate found")
    
    # Calculate true azimuth for comparison
    mic_center = np.mean(mic_positions[:2, :], axis=1)
    source_rel = true_source_pos[:2] - mic_center
    true_azimuth_deg = np.rad2deg(np.arctan2(source_rel[0], source_rel[1]))
    print(f"\n{'True Source':12s}: {true_azimuth_deg:>6.2f}° (azimuth)")


if __name__ == "__main__":
    main()

