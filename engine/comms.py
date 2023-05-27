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
        socket = serial.Serial(port=port, baudrate=baud_rate, timeout=20)
    except serial.SerialException as e:
        print(f'Could not connect to {port}. Check the port name and baud rate and try again.')
        raise
    return socket


def send_move_to_arm(socket, move: tuple[str,str] | tuple[int,int], capture: bool) -> None:
    """
    Encodes the move to bytes and sends move to arm.
    The move is encoded in the format "<start_square>,<end_square>"
    """
    if type(move[0]) == 'int':
        _ = (pos_to_coords[int(log2(move[0]))].lower(), pos_to_coords[int(log2(move[1]))].lower())
    else:
        _ = (move[0], move[1])

    if capture:
        move_str = f'{_[0]}x{_[1]}'
    else:
        move_str = f'{_[0]},{_[1]}'
    socket.write(move_str.encode())


def get_move_from_arm(socket) -> tuple[str,str]:
    """
    Blocks for the arm to send a move to the computer and
    returns the move converted to the appropriate format
    depending on the engine.

    Expects the arm to send the move by sending the start
    square first, then the end square.
    """
    try:
        start = pos_to_coords[int(socket.read(4))].lower()
        end = pos_to_coords[int(socket.read(4))].lower()
    except ValueError:
        print('Didn\'t receive any data from the arm')
        raise
    return start, end
