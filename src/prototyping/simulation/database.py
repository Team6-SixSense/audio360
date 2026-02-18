
import shutil
import os
from pydub import AudioSegment


def save_audio_data(audio_data: AudioSegment, filename: str, dir_name: str) -> bool:
    """
    Saves provided AudioSegment data to local data folder with prefix and filename.

    :param audio_data: The audio data of type AudioSegment.
    :param filename: The filename of the file to be saved.
    :param dir_name: The folder in the local data folder before the filename.
    :return: Boolean value representing success of save operation.
    """

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
    """
    Deletes audio files from local data folder.

    :param dir_name: The folder in the data folder to be removed.
    :return: Boolean representing success of deletion operation.
    """

    try:
        base_dir = os.path.join(os.path.dirname(__file__), "data", dir_name)
        shutil.rmtree(base_dir)
        return True
    except Exception as e:
        print(f"Error deleting audio data: {e}")
        return False


def load_audio_data(dir_name: str, filename: str) -> AudioSegment:
    """
    Loads audio data from local data folder as an AudioSegment python object.

    :param dir_name: The folder name in the local data folder to retrieve the audio from.
    :param filename: The filename of the audio that is to be retrieved.
    :return: AudioSegment object containing audio data.
    """

    filepath = os.path.join(os.path.dirname(__file__), "data", dir_name, filename)
    try:
        return AudioSegment.from_mp3(filepath)
    except Exception as e:
        print(f"Error loading audio data: {e}")
        return None


def write_readme(dir_name: str, content: str) -> bool:
    """
    Saves the description of the test case to the local data folder.
    """

    try:
        with open(os.path.join(os.path.dirname(__file__), "data", dir_name, "README.md"), "w") as f:
            f.write(content)
            return True
    except Exception as e:
        print(f"Error writing README.md: {e}")
        return False