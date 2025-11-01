#!/usr/bin/env python3
"""
Audio Resampling Utility

This script allows you to check the sampling frequency of MP3 files
and resample them to match your simulation's target sampling rate.

Usage:
    # Check sampling rate
    python resample_audio.py --check input.mp3
    
    # Resample to 8000 Hz and convert to mono
    python resample_audio.py --input input.mp3 --output output.mp3 -m --target-fs 8000
"""

import argparse
import sys
from pathlib import Path
from pydub import AudioSegment


def check_audio_info(file_path: str) -> None:
    """
    Display information about an audio file.
    
    :param file_path: path to the audio file
    """
    try:
        audio = AudioSegment.from_mp3(file_path)
        
        print(f"Sampling Rate: {audio.frame_rate} Hz")
        print(f"Channels: {audio.channels}")
        print(f"Sample Width: {audio.sample_width} bytes")
        print(f"Duration: {len(audio) / 1000:.2f} seconds")
        print(f"Frame Count: {audio.frame_count()} frames")
        print(f"Frame Width: {audio.frame_width} bytes")
        
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        sys.exit(1)
    except Exception as e:
        print(f"Error reading file: {e}")
        sys.exit(1)


def resample_audio(input_path: str, output_path: str, target_fs: int, 
                   to_mono: bool = False) -> None:
    """
    Resample an audio file to a target sampling frequency.
    
    :param input_path: path to input audio file
    :param output_path: path to save resampled audio
    :param target_fs: target sampling frequency in Hz
    :param to_mono: convert to mono if True (default: False)
    """
    try:
        # Load the audio file
        print(f"Loading '{input_path}'...")
        audio = AudioSegment.from_mp3(input_path)
        
        original_fs = audio.frame_rate
        original_duration = len(audio) / 1000
        
        print(f"\nOriginal:")
        print(f"Sampling Rate: {original_fs} Hz")
        print(f"Duration: {original_duration:.2f} seconds")
        print(f"Channels: {audio.channels}")
        
        # Convert to mono if requested
        if to_mono and audio.channels > 1:
            print(f"\nConverting to mono...")
            audio = audio.set_channels(1)
        
        # Resample
        if original_fs != target_fs:
            print(f"\nResampling from {original_fs} Hz to {target_fs} Hz...")
            audio = audio.set_frame_rate(target_fs)
        else:
            print(f"\nSampling rate already at target {target_fs} Hz, no resampling needed.")
        
        # Save the resampled audio
        print(f"\nSaving to '{output_path}'...")
        audio.export(output_path, format="mp3")
        
        new_duration = len(audio) / 1000
        
        print(f"\nResampled:")
        print(f"Sampling Rate: {audio.frame_rate} Hz")
        print(f"Duration: {new_duration:.2f} seconds")
        print(f"Channels: {audio.channels}")
        
    except FileNotFoundError:
        print(f"Error: File '{input_path}' not found.")
        sys.exit(1)
    except Exception as e:
        print(f"Error processing audio: {e}")
        sys.exit(1)


def main():
    """Main entry point for the script."""
    parser = argparse.ArgumentParser(
        description="Check and resample MP3 audio files",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    
    parser.add_argument(
        "--check", "-c",
        type=str,
        metavar="FILE",
        help="Check audio file information without resampling"
    )
    
    parser.add_argument(
        "--input", "-i",
        type=str,
        metavar="INPUT",
        help="Input audio file path"
    )
    
    parser.add_argument(
        "--output", "-o",
        type=str,
        metavar="OUTPUT",
        help="Output audio file path"
    )
    
    parser.add_argument(
        "--target-fs", "-t",
        type=int,
        metavar="FREQUENCY",
        default=8000,
        help="Target sampling frequency in Hz (default: 8000)"
    )
    
    parser.add_argument(
        "--mono", "-m",
        action="store_true",
        help="Convert to mono audio"
    )
    
    args = parser.parse_args()
    
    # Check mode
    if args.check:
        check_audio_info(args.check)
        return
    
    # Resample mode
    if args.input and args.output:
        resample_audio(args.input, args.output, args.target_fs, args.mono)
        return
    
    # If neither mode is specified, show help
    parser.print_help()
    sys.exit(1)


if __name__ == "__main__":
    main()

