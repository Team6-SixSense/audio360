"""
This file provides sample usage on creating audio sources that generate simple sine waves
audios and capturing the audio responses on multiple microphone within a room using
pyroomacoustics. 
"""

from collections import deque
import os
from simulation import (create_room,
                           add_microphone,
                           generate_sine_wave,
                           add_source_with_signal,
                           plot_room,
                           save_room_data,
                           classify_audio,
                           _format_readme)

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
    sine_wave_frequency_hz = 1000
    duration_s = 1.0

    CHUNK = 16000
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

    source = [5.0, 5.5, 1.75]
    print(f"Adding source at {source}")
    room.add_source(source) 

    # Compute RIR for each microphone that is defined. 
    # rir is a length of 4 microphones in this case. 
    room.compute_rir()
    rirs = room.rir

    # This is just initializing how we will stream the microphone data. 
    p = pyaudio.PyAudio()
    stream = p.open(format=FORMAT, channels=CHANNELS, rate=RATE,
                input=True, frames_per_buffer=CHUNK)

    # Initiaze the first audio source chunk
    source_chunk = np.frombuffer(stream.read(CHUNK), dtype=np.float32)

    mic_buffers = [np.zeros(len(rirs[m][0]) + CHUNK - 1) for m in range(len(rirs))]
    output_deques = [deque(maxlen=CHUNK) for _ in range(len(rirs))]


    # chunk_index = 0
    while True:
        source_chunk = np.frombuffer(stream.read(CHUNK), dtype=np.float32)

        rms = np.sqrt(np.mean(source_chunk**2))
        if rms < 0.005:
            print("Predicted: uncertain (silence)")
            continue

        mic_outputs = []
        for m, rir in enumerate(rirs):
            # Single RIR: rir[m][0]
            conv = np.convolve(source_chunk, rir[0])
            
            # Overlap-add into mic buffer
            output_deques[m].extend(conv)

            audio_segment = np.array(output_deques[m], dtype=np.float32)

            [pred, prob] = classify_audio(audio_segment)
            pred_prob = np.max(prob)
            if (pred_prob < 0.7):
                print("Predicted: uncertain (silence)")
            else:            
                print(f"Prediction {pred}")
                print(f"Probability {prob}")

            mic_buffers[m] = mic_buffers[m] + conv
            out = mic_buffers[m][:CHUNK]
            mic_buffers[m] = np.roll(mic_buffers[m], -CHUNK)
            
            mic_outputs.append(out)

        # outputAudioToFile(mic_outputs, chunk_index, RATE)
        # chunk_index+=1




    print("Generating sine wave signals")
    signal1 = generate_sine_wave(sine_wave_frequency_hz, duration_s, fs_hz)
    signal2 = generate_sine_wave(sine_wave_frequency_hz * 1.5, duration_s, fs_hz)

    print("Adding sources with signal")
    add_source_with_signal(room, [9, 9, 1], signal1)
    add_source_with_signal(room, [1, 7, 2.5], signal2)

    print("Simulating room")
    room.simulate()

    plot_room(room)

    print("Saving audio data")
    save_room_data(room, "example_sine_wave", 
                         "This is a test case for the pyroomacoustics_prototype module with sine wave audio sources.")

    print("Simulation complete!")

if __name__ == "__main__":
    main()
