"""
This file provides sample usage on creating audio sources that generate simple sine waves
audios and capturing the audio responses on multiple microphone within a room using
pyroomacoustics. 
"""

import warnings
warnings.filterwarnings("ignore")

import os
import math
from simulation import (create_room,
                           add_microphone
                           )

import numpy as np
import soundfile as sf
import scipy.signal



def outputAudioToFile(mic_outputs, chunk_index, RATE):
    for m, out in enumerate(mic_outputs):
        filename = f"mic_recordings/mic{m}_chunk{chunk_index}.wav"
        sf.write(filename, out, RATE)



def main():
    """
    Example usage for showing how to work with pyroomacoustics. 
    
    This example generates sample .mp3 files that represent the audio response on multiple 
    micrphones after a sine wave audio source is emitted within a simulated room.
    """

    # Simulation parameters
    CHUNK = 1024
    CAPTURE_SECONDS = 2
    RATE = 16000

    # Ensure output directory exists
    if not os.path.exists("mic_recordings"):
        os.makedirs("mic_recordings")

    # Mic array center and rotation parameters
    center_x = 5.0
    center_y = 5.05
    radius = 3.0
    source_z = 1.0

    # Generate 440Hz sine wave source
    t = np.arange(RATE * CAPTURE_SECONDS) / RATE
    source_signal = 0.5 * np.sin(2 * np.pi * 440 * t).astype(np.float32)

    print("Starting 360 degree simulation...")

    for angle in range(0,361,45):
        # Calculate source position
        rad = math.radians(angle)
        src_x = center_x + radius * math.cos(rad)
        src_y = center_y + radius * math.sin(rad)
        true_source_pos = [src_x, src_y, source_z]
        
        print(f"Angle: {angle}, Source: {true_source_pos}")

        # Create room for this angle
        room = create_room(10, 10, 3, RATE)

        # Add microphones
        add_microphone(room, [4.95, 5.10, 1.75]) # Front-left
        add_microphone(room, [5.05, 5.10, 1.75]) # Front-right
        add_microphone(room, [4.95, 5.00, 1.75]) # Back-left
        add_microphone(room, [5.05, 5.00, 1.75]) # Back-right

        # Add source
        room.add_source(true_source_pos)

        # Compute RIR for each microphone that is defined. 
        room.compute_rir()
        rirs = room.rir

        # Convolve source signal with RIRs
        mic_outputs = []
        for m_rirs in rirs:
            # Convolve with the first source's RIR
            convolved = scipy.signal.fftconvolve(source_signal, m_rirs[0], mode='full')
            # Trim to original signal length
            mic_outputs.append(convolved[:len(source_signal)])
            
        outputAudioToFile(mic_outputs, angle, RATE)



if __name__ == "__main__":
    main()
