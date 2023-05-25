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
        help='Set the path to the stockfish executable',
        dest='path'
    )
    parser.add_argument(
        '-c',
        '--port',
        required=True,
        help='Set the port name that the arm\'s microcontroller is connected to',
        dest='port'
    )
    parser.add_argument(
        '-b',
        '--baud-rate',
        default=9600,
        type=int,
        help='Set the baud rate for communication with Arduino',
        dest='baud'
    )

    args = parser.parse_args()

    socket = comms.get_socket(args.port, args.baud)

    player_side = input('Do you want to play white or black (w/b)?')
    while player_side not in ['w', 'b']:
        print('Enter "w" for white and "b" for black.')
        player_side = input('Do you want to play white or black (w/b)?')

    player_side = 'white' if player_side == 'w' else 'black'
    board_side = 'black' if player_side == 'white' else 'black'

    board = Board(board_side)
    engine = stockfishpy.Engine(args.path)
    engine.ucinewgame()
    print(engine.isready())
    
    side_to_move = player_side if player_side == 'white' else board_side
    lines_printed = 11
    print(board)
    while True:
        clear_lines(lines_printed)
        print(board)
        lines_printed = 11
        if side_to_move == board.side:
            # Find move and make it (send it to arm)
            if args.engine == 'default':
                _, best_move = board.search_forward(args.depth)
                board.move(best_move[0], best_move[1])
                comms.send_move_to_arm(socket, (best_move[0], best_move[1]))
            else:
                # Stockfish always makes the best moves
                best_move = engine.bestmove()['bestmove']
                engine.setposition([best_move])
                comms.send_move_to_arm(socket, (best_move[0:2], best_move[2:]))
        else:
            # Read move to make from serial port
            pass
            
        side_to_move = 'white' if side_to_move == 'black' else 'black'


if __name__ == '__main__':
    main()
