"""
Room construction helpers.
"""

from __future__ import annotations

from typing import Sequence

import pyroomacoustics as pra
import numpy as np


Position = Sequence[float]


def create_room(room_dim_x: float, room_dim_y: float, room_dim_z: float, fs: int) -> pra.ShoeBox:
    """
    Create a room with the given dimensions and sampling frequency.

    :param room_dim_x: The x dimension of the room.
    :param room_dim_y: The y dimension of the room.
    :param room_dim_z: The z dimension of the room.
    :param fs: The sampling frequency of the room.
    :return: A ShoeBox object representing the room.
    """

    return pra.ShoeBox([room_dim_x, room_dim_y, room_dim_z], fs=fs)


def add_microphone(room: pra.ShoeBox, microphone_position: list) -> pra.ShoeBox:
    """
    Add a microphone to the room at the given position.

    :param room: The ShoeBox object representing the room.
    :param microphone_position: The position of the microphone as a list of [x, y, z] coordinates.
    :return: The ShoeBox object representing the room.
    """

    room.add_microphone(microphone_position)
    return room


def add_source_with_signal(room: pra.ShoeBox, source_position: list, signal: np.ndarray) -> pra.ShoeBox:
    """
    Add a source to the room with the given position and signal.

    :param room: The ShoeBox object representing the room.
    :param source_position: The position of the source as a list of [x, y, z] coordinates.
    :param signal: The signal to be added to the source.
    :return: The ShoeBox object representing the room.
    """

    room.add_source(source_position, signal=signal)
    return room
