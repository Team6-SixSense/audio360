"""
This file generates DOA test audio files that match the configuration in 
test/audio/mic_recordings/README.md for automated testing of the DOA algorithm.

The audio files are saved as WAV files with the naming convention:
mic{mic_num}_angle_{angle}.wav where mic_num is 0-3 and angle is 0-360.
"""

import sys
import os
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from pyroom_helper import create_room, add_microphone, generate_sine_wave, add_source_with_signal
import numpy as np
import soundfile as sf


def main():
    """
    Generate DOA test audio files for multiple source angles.
    
    Configuration matches test/audio/mic_recordings/README.md:
    - Room: 20m x 20m x 3m
    - Sampling frequency: 16000 Hz
    - Microphones arranged in a square around center (10.0, 10.05, 1.75)
    - Sound sources at 3m distance from mic center at various angles
    - Angle convention: 0° = North (+Y), 90° = West (-X), clockwise from North
    """
    
    # Simulation parameters (matching test README)
    fs_hz = 16000
    room_dim = [20.0, 20.0, 3.0]
    
    # Microphone positions (matching test README exactly)
    # These form a rectangular array centered approximately at (10.0, 10.05, 1.75)
    mic_positions = [
        [9.95, 10.10, 1.75],   # Microphone 1 (mic0) - Front-left
        [10.05, 10.10, 1.75],  # Microphone 2 (mic1) - Front-right
        [10.05, 10.00, 1.75],  # Microphone 3 (mic2) - Back-right
        [9.95, 10.00, 1.75],   # Microphone 4 (mic3) - Back-left
    ]
    
    # Calculate microphone array center
    mic_center = np.mean(mic_positions, axis=0)
    print(f"Microphone array center: ({mic_center[0]:.2f}, {mic_center[1]:.2f}, {mic_center[2]:.2f})")
    
    # Test angles (in degrees) - 0° = North, 90° = West (clockwise from North)
    test_angles_deg = [0, 45, 90, 135, 180, 225, 270, 315]
    
    # Source distance from microphone center (3 meters as per README)
    source_distance = 3.0
    
    # Generate sine wave signal (1 second at 1kHz for clear test signal)
    duration_s = 0.5  # 0.5 seconds for test audio
    sine_wave_frequency_hz = 1000
    signal = generate_sine_wave(sine_wave_frequency_hz, duration_s, fs_hz, amplitude=0.5)
    
    print(f"\nGenerating DOA test audio files:")
    print(f"  Room dimensions: {room_dim[0]}m x {room_dim[1]}m x {room_dim[2]}m")
    print(f"  Sampling rate: {fs_hz} Hz")
    print(f"  Signal: {sine_wave_frequency_hz} Hz sine wave, {duration_s}s duration")
    print(f"  Source distance: {source_distance}m from mic center")
    print(f"  Test angles: {test_angles_deg}")
    
    # Create output directory
    output_dir = "../../../../test/audio/mic_recordings_new"
    os.makedirs(output_dir, exist_ok=True)
    
    # Process each angle
    for angle_deg in test_angles_deg:
        print(f"\n--- Processing angle: {angle_deg}° ---")
        
        # Convert angle to radians and calculate source position
        # Coordinate system: 0° = North (+Y), 90° = West (-X), clockwise
        angle_rad = np.deg2rad(angle_deg)
        
        # Calculate source position (clockwise from North)
        # x increases to East, y increases to North
        # For clockwise from North: x = center_x - distance * sin(angle)
        #                          y = center_y + distance * cos(angle)
        source_x = mic_center[0] - source_distance * np.sin(angle_rad)
        source_y = mic_center[1] + source_distance * np.cos(angle_rad)
        source_z = mic_center[2]  # Same height as mics
        
        source_position = [source_x, source_y, source_z]
        print(f"  Source position: ({source_x:.2f}, {source_y:.2f}, {source_z:.2f})")
        
        # Create room for this angle
        room = create_room(room_dim[0], room_dim[1], room_dim[2], fs_hz)
        
        # Add all microphones
        for mic_pos in mic_positions:
            add_microphone(room, mic_pos)
        
        # Add source with signal
        add_source_with_signal(room, source_position, signal)
        
        # Simulate room acoustics
        room.simulate()
        
        # Save each microphone's recording as a separate WAV file
        # Normalize to int16 range
        signals = room.mic_array.signals  # shape: (n_mics, n_samples)
        
        for mic_idx in range(signals.shape[0]):
            mic_signal = signals[mic_idx, :]
            
            # Normalize to [-1, 1] range
            peak = float(np.max(np.abs(mic_signal))) if mic_signal.size > 0 else 1.0
            if peak == 0:
                peak = 1.0
            normalized_signal = mic_signal / peak
            
            # Save as WAV file (16-bit PCM) with 0-indexed naming
            filename = f"{output_dir}/mic{mic_idx}_angle_{angle_deg}.wav"
            sf.write(filename, normalized_signal, fs_hz, subtype='PCM_16')
            
            # ALSO save with 1-indexed naming for backward compatibility
            # (gccPhat_test.cpp expects mic1, mic2, mic3, mic4)
            if mic_idx < 4:
                filename_1indexed = f"{output_dir}/mic{mic_idx + 1}_angle_{angle_deg}.wav"
                sf.write(filename_1indexed, normalized_signal, fs_hz, subtype='PCM_16')
            
        print(f"  Saved: mic0-3 and mic1-4 (both indexed) angle_{angle_deg}.wav")
    
    # Create README file
    readme_content = f"""# Microphone recordings data

The audio files in the folder holds a recording of audio from a single source.
It was generated using pyroomacoustics.

## Room Dimensions

- X: {room_dim[0]:.2f} m
- Y: {room_dim[1]:.2f} m
- Z: {room_dim[2]:.2f} m

## Sampling Frequency

{fs_hz} Hz

## Microphone Positions

- Microphone 1: ({mic_positions[0][0]:.2f}, {mic_positions[0][1]:.2f}, {mic_positions[0][2]:.2f})
- Microphone 2: ({mic_positions[1][0]:.2f}, {mic_positions[1][1]:.2f}, {mic_positions[1][2]:.2f})
- Microphone 3: ({mic_positions[2][0]:.2f}, {mic_positions[2][1]:.2f}, {mic_positions[2][2]:.2f})
- Microphone 4: ({mic_positions[3][0]:.2f}, {mic_positions[3][1]:.2f}, {mic_positions[3][2]:.2f})

## Sound Sources

There is one sound source located {source_distance} m away from the center of the microphone array.
The relative angle is in the file name, where 0 degrees is North and 90 degrees is West.

File naming convention: ```mic{{mic_num}}_angle_{{angle}}.wav```
"""
    
    readme_path = f"{output_dir}/README.md"
    with open(readme_path, 'w') as f:
        f.write(readme_content)
    
    print(f"\n✓ Generation complete! Files saved to: {output_dir}/")
    print(f"✓ README.md created")
    print(f"\nTo test: Copy files from {output_dir}/ to test/audio/mic_recordings/")


if __name__ == "__main__":
    main()
