from math import log2
from PIL import Image

import vision

from chessengine import Board
from chessengine.lookup_tables import pos_to_coords


square_names = {'a1', 'a2', 'a3', 'a4', 'a5', 'a6', 'a7', 'a8', 'b1', 'b2', 'b3', 'b4', 'b5', 'b6', 'b7', 'b8', 'c1',
                'c2', 'c3', 'c4', 'c5', 'c6', 'c7', 'c8', 'd1', 'd2', 'd3', 'd4', 'd5', 'd6', 'd7', 'd8', 'e1', 'e2',
                'e3', 'e4', 'e5', 'e6', 'e7', 'e8', 'f1', 'f2', 'f3', 'f4', 'f5', 'f6', 'f7', 'f8', 'g1', 'g2', 'g3',
                'g4', 'g5', 'g6', 'g7', 'g8', 'h1', 'h2', 'h3', 'h4', 'h5', 'h6', 'h7', 'h8'}


def get_moves_made(board):
    """
    Returns a list of all moves made on the board so far in UCI compliant format
    to set Stockfish's position.
    """
    moves = []
    for move in board.moves:
        moves.append(pos_to_coords[int(log2(move[0]))].lower() + pos_to_coords[int(log2(move[1]))].lower())
    return moves


def detect_move_made(board: Board, prev_state: Image, curr_state: Image) -> list[str]:
    """
    Takes the previous and current state image and detects the piece that moved on the board.
    If CV fails, verifies with Gemini.
    Returns a list of coordinates (for e.g. ["C2", "C3"]).
    """
    move = vision.detect_move_made(prev_state, curr_state)
    return move
