"""
Communication module for communicating between the Arduino
controller of the arm connected to a Serial port and the engine.
"""
from math import log2

import serial
from chessengine.lookup_tables import pos_to_coords


def get_socket(port: str, baud_rate: int):
    """
    Creates a socket connection at the given port at the given baud rate,
    opens the socket, and returns the socket.
    """
    try:
        socket = serial.Serial(port=port, baudrate=baud_rate)
    except serial.SerialException as e:
        print(f'Could not connect to {port}. Check the port name and baud rate and try again.')
        raise
    return socket


def send_move_to_arm(socket, move: tuple[str,str] | tuple[int,int]) -> None:
    """
    Encodes the move to bytes and sends move to arm.
    """
    if type(move[0]) == 'int':
        _ = (pos_to_coords[int(log2(move[0]))].lower(), pos_to_coords[int(log2(move[1]))].lower())
    else:
        _ = (move[0], move[1])

    move_str = f'{_[0]},{_[1]}'
    socket.write(move_str.encode())
