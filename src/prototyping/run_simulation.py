"""
This file provides sample usage on creating audio sources that generate simple sine waves
audios and capturing the audio responses on multiple microphone within a room using
pyroomacoustics. 
"""

import warnings
warnings.filterwarnings("ignore")

from collections import deque
import os
from simulation import (create_room,
                           add_microphone,
                           classify_audio,
                           analyze_doa_continuous,
                           capture_audio_from_source
                           )
from visualization import visualize_glasses_continuous, init_glasses_gui

import numpy as np
import pyaudio
import soundfile as sf



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
    BUFFER_SECONDS = 4
    FORMAT = pyaudio.paFloat32
    CHANNELS = 1
    RATE = 16000


    print("Creating room")
    room = create_room(10, 10, 3, RATE)

    print("Adding microphones")
    add_microphone(room, [4.95, 5.10, 1.75]) # Front-left
    add_microphone(room, [5.05, 5.10, 1.75]) # Front-right
    add_microphone(room, [4.95, 5.00, 1.75]) # Back-left
    add_microphone(room, [5.05, 5.00, 1.75]) # Back-right

    mic_positions = np.array([
        [4.95, 5.05, 4.95, 5.05],  # x coordinates (meters)
        [5.10, 5.10, 5.00, 5.00],  # y coordinates (meters)
        [1.75, 1.75, 1.75, 1.75]   # z coordinates (meters)
    ])

    true_source_pos = np.array([1.0, 6.0, 1.0])
    print(f"Adding source at {true_source_pos}")
    room.add_source(true_source_pos) 

    # Compute RIR for each microphone that is defined. 
    # rir is a length of 4 microphones in this case. 
    room.compute_rir()
    rirs = room.rir

    # This is just initializing how we will stream the microphone data. 
    p = pyaudio.PyAudio()
    stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE,
                input=True, frames_per_buffer=CHUNK)

    # Initialize the first audio source chunk
    source_chunk = np.frombuffer(stream.read(CHUNK), dtype=np.float32)

    # Will be used as the buffer for each of our microphones. BUffer will be CHUNK * BUFFER_SECONDS
    output_deques = [deque(maxlen=BUFFER_SECONDS * CHUNK) for _ in range(len(rirs))]

    # Initialize glasses GUI before starting the main loop
    print("Initializing glasses visualization...")
    init_glasses_gui()
    print("Glasses GUI ready. Starting audio processing...")

    while True:
        # Continuosly retrieved the audio stream of size chunk from device microphone.
        source_chunk = np.frombuffer(stream.read(CHUNK, exception_on_overflow=False), dtype=np.float32)

        # Skip sounds that are faint/quiet. 
        rms = np.sqrt(np.mean(source_chunk**2))
        if rms < 0.001:  # Lowered threshold to be more sensitive
            # Print RMS value for debugging (only occasionally to avoid spam)
            if np.random.random() < 0.01:  # Print ~1% of the time
                print(f"RMS: {rms:.6f} (too quiet)")
            continue
        
        print(f"Sound detected! RMS: {rms:.6f}")

        # Capture audio from source to all 4 microphones
        output_deques = capture_audio_from_source(source_chunk, rirs, output_deques, CHUNK)

        # Classify the audio retrieved from the first microphone
        mic1_input = np.array(output_deques[0], dtype=np.float32)
        try:
            pred = classify_audio(mic1_input)
            print(f"Prediction: {pred}")
        except Exception as e:
            print(f"Classification error: {e}")
            pred = "Undetermined"

        # Calculate direction of audio
        try:
            results = analyze_doa_continuous(
                output_deques,
                mic_positions=mic_positions,
                true_source_position=true_source_pos,
                algorithms=['frida', 'srp', 'music'] 
            )
            # Print DOA results for debugging
            for algo_name, doa_obj in results.items():
                if doa_obj.azimuth_recon is not None and len(doa_obj.azimuth_recon) > 0:
                    print(f"  {algo_name}: {np.rad2deg(doa_obj.azimuth_recon[0]):.1f}°")
        except Exception as e:
            print(f"DOA analysis error: {e}")
            results = {}

        # Update glasses visualization with classification and direction
        # Uses moving average for classification (4/5 threshold) and smoothing for direction
        visualize_glasses_continuous(results, pred, mic_positions, true_source_pos)



if __name__ == "__main__":
    main()
