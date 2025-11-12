"""
Script that will read raw audio data and convert into a WAV file.
"""

import argparse
import wave

import numpy as np

def main():
    """ Main entry function. """

    # Argument parsing.
    print("Parsing arguments and setting configurations.")
    parser = argparse.ArgumentParser(description="Raw PCM data to WAV file converter.")
    parser.add_argument("-i", "--input_file", type=str, help="Input file with PCM data." +
                        " Must follow file format defined in README.md.")
    parser.add_argument("-o", "--wav_filename", type=str, help="Output WAV file name.")
    parser.add_argument("--number_channels", type=int, default=1,
                        help="Number of channels. 1: Mono, 2: Stereo.")
    parser.add_argument("--sample_width", type=int, default=4,
                        help="Sample width. 2: 16 bits, 4: 32 bits.")
    parser.add_argument("--sample_rate", type=int, default=44100,
                        help="Sample rate. Number samples per second.")

    args = parser.parse_args()
    input_file = args.input_file
    wav_filename = args.wav_filename
    number_channels = args.number_channels
    sample_width = args.sample_width
    sample_rate = args.sample_rate

    # Read in raw audio data.
    print("Loading input data and processing it.")
    audio_data = np.loadtxt(input_file, delimiter=',', dtype=float)

    # Convert to correct n-bit integers.
    if sample_width == 4:
        audio_data = (audio_data * np.iinfo(np.int32).max).astype(np.int32)
    elif sample_width == 2:
        audio_data = (audio_data * np.iinfo(np.int16).max).astype(np.int16)
    else:
        raise NotImplementedError("Unsupported sample width.")

    # Create the WAV file.
    print("Creating WAV file.")
    with wave.open(wav_filename, 'wb') as wav_file:
        wav_file.setnchannels(number_channels)
        wav_file.setsampwidth(sample_width)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(audio_data.tobytes())
        print(f"Finished saving data to WAV file: {wav_filename}")

    print("Creating WAV file completed.")

if __name__ == "__main__":
    main()
