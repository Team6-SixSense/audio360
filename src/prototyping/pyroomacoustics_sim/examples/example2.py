"""
This file provides sample usage on creating audio sources that generate audio from a provided
.mp3 file, and capturing the audio responses on multiple microphone within a room using
pyroomacoustics.
"""

from pyroom_helper import (create_room,
                           add_microphone,
                           generate_signal_from_audio_file,
                           add_source_with_signal,
                           plot_room,
                           save_room_data)

def main():
    """
    Example usage for showing how to work with pyroomacoustics.
    
    This example generates sample .mp3 files that represent the audio response on multiple 
    micrphones after an audio source is emitted using a pre-existing audio file (.mp3)
    within a simulated room.
    """

    # Simulation parameters
    fs_hz = 8000

    print("Creating room")
    room = create_room(10, 10, 3, fs_hz)

    print("Adding microphones")
    add_microphone(room, [4.95, 5.10, 1.75]) # Front-left
    add_microphone(room, [5.05, 5.10, 1.75]) # Front-right
    add_microphone(room, [4.95, 5.00, 1.75]) # Back-left
    add_microphone(room, [5.05, 5.00, 1.75]) # Back-right

    print("Generating car sound from audio file")
    signal1 = generate_signal_from_audio_file("example_audio", "car_horn.mp3")

    print("Adding sources with signal")
    add_source_with_signal(room, [9, 9, 1], signal1)

    print("Simulating room")
    room.simulate()

    plot_room(room)

    print("Saving audio data")
    save_room_data(room, "example_mp3_audio_sources", 
                         "This is a test case for the pyroomacoustics_prototype module with audio sources from .mp3 files.")

    print("Simulation complete!")

if __name__ == "__main__":
    main()
