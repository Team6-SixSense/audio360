"""
Script that will read raw audio data and convert into a WAV file.
"""

import argparse
import struct
import wave


# 4 bytes constants.
STRUCT_FORMAT_4_BYTES = '<i'
MAX_POSITIVE_VALUE_4_BYTES = 2147483647
MAX_NEGATIVE_VALUE_4_BYTES = -2147483648

# 2 bytes constants
STRUCT_FORMAT_2_BYTES = '<h'
MAX_POSITIVE_VALUE_2_BYTES = 32767
MAX_NEGATIVE_VALUE_2_BYTES = -32768

def read_audio_bin_file(input_file: str, sample_width_bytes: int) -> bytearray:
    """
    Reads a binary audio file and locally store it.
    Read the README at this project level for expected file format.
    
    :param input_file: The input file path.
    :param sample_width_bytes: The number of bytes per sample.
    :return: A bytearray representing the audio data from the binary file.
    """

    audio_data = bytearray()
    struct_format = ''
    max_positive_value = 0
    max_negative_value = 0

    if sample_width_bytes == 4:
        struct_format = STRUCT_FORMAT_4_BYTES
        max_positive_value = MAX_POSITIVE_VALUE_4_BYTES
        max_negative_value = MAX_NEGATIVE_VALUE_4_BYTES

    elif sample_width_bytes == 2:
        struct_format = STRUCT_FORMAT_2_BYTES
        max_positive_value = MAX_POSITIVE_VALUE_2_BYTES
        max_negative_value = MAX_NEGATIVE_VALUE_2_BYTES

    with open(input_file, 'rb') as raw_file:
        while True:
            sample_bytes = raw_file.read(sample_width_bytes)

            # Stop when we reach the end of the file.
            if not sample_bytes:
                break

            # Make sure we read a full sample.
            if len(sample_bytes) < sample_width_bytes:
                print("Warning: Incomplete sample at end of file. Skipping.")
                continue

            # Unpack the bytes into a single integer
            sample_int, = struct.unpack(struct_format, sample_bytes)

            # Amplify it by bit-shifting left 8 bits. This is because microphone data is
            # represented as 24 bits but stored as 32 bits.
            amplified_sample_int = sample_int << 8

            try:
                # Pack the new, louder integer back into 4 bytes
                amplified_sample_bytes = struct.pack(struct_format, amplified_sample_int)
            except struct.error:
                print(f"Error: Value {amplified_sample_int} overflowed 32-bit size.")
                print("Clipping to max/min value.")

                if amplified_sample_int > 0:
                    amplified_sample_int = max_positive_value
                else:
                    amplified_sample_int = max_negative_value

                amplified_sample_bytes = struct.pack(struct_format, amplified_sample_int)

            # Add the new bytes to final data
            audio_data.extend(amplified_sample_bytes)

    return audio_data

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
    parser.add_argument("--sample_width_bytes", type=int, default=4,
                        help="Sample width in bytes. 2 bytes: 16 bits, 4 bytes: 32 bits.")
    parser.add_argument("--sample_rate", type=int, default=16000,
                        help="Sample rate. Number samples per second.")

    args = parser.parse_args()
    input_file = args.input_file
    wav_filename = args.wav_filename
    number_channels = args.number_channels
    sample_width_bytes = args.sample_width_bytes
    sample_rate = args.sample_rate

    if (sample_width_bytes != 4 and sample_width_bytes != 2):
        raise ValueError("Sample width bytes can be 2 or 4. "
                         f"Provided is {sample_width_bytes}")

    # Read in raw audio data.
    print("Loading input data and processing it.")
    audio_data = read_audio_bin_file(input_file, sample_width_bytes)

    # Create the WAV file.
    print("Creating WAV file.")
    with wave.open(wav_filename, 'wb') as wav_file:
        wav_file.setnchannels(number_channels)
        wav_file.setsampwidth(sample_width_bytes)
        wav_file.setframerate(sample_rate)
        wav_file.writeframes(audio_data)
        print(f"Finished saving data to WAV file: {wav_filename}")

    print("Creating WAV file completed.")

if __name__ == "__main__":
    main()
