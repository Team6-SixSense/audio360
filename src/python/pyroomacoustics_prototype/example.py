from pyroom_helper import (create_room, 
                           add_microphone, 
                           generate_sine_wave, 
                           add_source_with_signal, 
                           plot_room, 
                           save_room_data)

def main():
    # Simulation parameters
    fs = 8000 
    frequency = 1000 
    duration = 1.0 
    
    print("Creating room")
    room = create_room(10, 10, 3, fs)

    print("Adding micriphones")
    add_microphone(room, [4.95, 5.10, 1.75]) # Front-left
    add_microphone(room, [5.05, 5.10, 1.75]) # Front-right
    add_microphone(room, [4.95, 5.00, 1.75]) # Back-left
    add_microphone(room, [5.05, 5.00, 1.75]) # Back-right

    print("Generating sine wave signals")
    signal1 = generate_sine_wave(frequency, duration, fs)
    signal2 = generate_sine_wave(frequency * 1.5, duration, fs)
    
    print("Adding sources with signal")
    add_source_with_signal(room, [9, 9, 1], signal1)
    add_source_with_signal(room, [1, 7, 2.5], signal2)

    print("Simulating room")
    room.simulate()
    
    plot_room(room)

    print("Saving audio data")
    save_room_data(room)

    print("Simulation complete!")

if __name__ == "__main__":
    main()
