"""
Communication module for communicating between the Arduino
controller of the arm connected to a Serial port and the engine.
"""
from math import log2

import serial
from chessengine.lookup_tables import pos_to_coords


COM_PORT = 'COM6'
BAUD_RATE = 9600


socket = serial.Serial(COM_PORT, BAUD_RATE)


def send_move_to_arm(move: tuple[str,str] | tuple[int,int]) -> None:
    """
    Encodes the move to bytes and sends move to arm.
    
    :param move: A 2-tuple of strings or ints
    """
    if type(move[0]) == 'int':
        _ = (pos_to_coords[int(log2(move[0]))].lower(), pos_to_coords[int(log2(move[1]))].lower())
    else:
        _ = (move[0], move[1])

    move_str = f'{_[0]},{_[1]}'
    socket.write(move_str.encode())
