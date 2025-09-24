from pydub import AudioSegment
import shutil
import os


def save_audio_data(audio_data: AudioSegment, filename: str, dir_name: str) -> bool:
    try:
        # Make the base directory
        base_dir = os.path.join(os.path.dirname(__file__), "data", dir_name)
        os.makedirs(base_dir, exist_ok=True)

        # Make the output path
        out_path = os.path.join(base_dir, filename)

        # Export the audio data
        audio_data.export(out_path, format="mp3", bitrate="192k")
        return True
    except Exception as e:
        print(f"Error saving audio data: {e}")
        return False


def delete_audio_data(dir_name: str) -> bool:
    try:
        base_dir = os.path.join(os.path.dirname(__file__), "data", dir_name)
        shutil.rmtree(base_dir)
        return True
    except Exception as e:
        print(f"Error deleting audio data: {e}")
        return False


def load_audio_data(dir_name: str, filename: str) -> AudioSegment:
    filepath = os.path.join(os.path.dirname(__file__), "data", dir_name, filename)
    try:
        return AudioSegment.from_mp3(filepath)
    except Exception as e:
        print(f"Error loading audio data: {e}")
        return None
