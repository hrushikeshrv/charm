import argparse
from functools import wraps
import sys

from chessengine import Board
from chessengine.utils import clear_lines
import comms
import stockfishpy


def handle_exit(f):
    """
    Catch exit and inform arm before terminating.
    """
    @wraps(f)
    def wrapper():
        try:
            f()
        except KeyboardInterrupt as e:
            print(f'\nDetected {e.__class__.__name__} {e}.')
            # TODO: Inform arm
            print('Exiting')
            sys.exit(1)
    return wrapper


@handle_exit
def main():
    parser = argparse.ArgumentParser(prog='woozy', description='A chess-playing robotic arm')
    parser.add_argument(
        '-e', 
        '--engine', 
        choices=['default', 'stockfish'], 
        default='default',
        help='The chess engine to use. Can be "default" for using chessengine or "stockfish" for stockfish.',
        dest='engine'
    )
    parser.add_argument(
        '-d',
        '--depth',
        choices=range(1, 16),
        default=5,
        type=int,
        help='Set the search depth. Can be between 1 and 15 (inclusive). Recommended depth for default engine is 4 or 5.',
        dest='depth'
    )
    parser.add_argument(
        '-p',
        '--path',
        default='stockfish/stockfish.exe',
        help='Sets the path to the stockfish executable',
        dest='path'
    )

    args = parser.parse_args()

    player_side = input('Do you want to play white or black (w/b)?')
    while player_side not in ['w', 'b']:
        print('Enter "w" for white and "b" for black.')
        player_side = input('Do you want to play white or black (w/b)?')

    player_side = 'white' if player_side == 'w' else 'black'
    board_side = 'black' if player_side == 'white' else 'black'

    board = Board(board_side)
    engine = stockfishpy.Engine(args.path)
    print(engine.isready())
    
    side_to_move = player_side if player_side == 'white' else board_side
    lines_printed = 11
    while True:
        if side_to_move == board.side:
            clear_lines(lines_printed)
            print(board)
            lines_printed = 11
            # Find move and make it (send it to arm)
            if args.engine == 'default':
                _, best_move = board.search_forward(args.depth)
                board.move(best_move[0], best_move[1])
                comms.send_move_to_arm((best_move[0], best_move[1]))
            else:
                pass
        else:
            # Read move to make from serial port
            pass


if __name__ == '__main__':
    main()
