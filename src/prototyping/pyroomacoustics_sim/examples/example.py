"""
This file provides sample usage on creating audio sources that generate simple sine waves
audios and capturing the audio responses on multiple microphone within a room using
pyroomacoustics. 
"""

from pyroom_helper import (create_room,
                           add_microphone,
                           generate_sine_wave,
                           add_source_with_signal,
                           plot_room,
                           save_room_data,
                           _format_readme)

def main():
    """
    Example usage for showing how to work with pyroomacoustics. 
    
    This example generates sample .mp3 files that represent the audio response on multiple 
    micrphones after a sine wave audio source is emitted within a simulated room.
    """

    # Simulation parameters
    fs_hz = 8000
    sine_wave_frequency_hz = 1000
    duration_s = 1.0

    print("Creating room")
    room = create_room(10, 10, 3, fs_hz)

    print("Adding microphones")
    add_microphone(room, [4.95, 5.10, 1.75]) # Front-left
    add_microphone(room, [5.05, 5.10, 1.75]) # Front-right
    add_microphone(room, [4.95, 5.00, 1.75]) # Back-left
    add_microphone(room, [5.05, 5.00, 1.75]) # Back-right

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
