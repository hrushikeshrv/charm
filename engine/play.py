import argparse
from functools import wraps
from math import log2
import sys

from chessengine import Board
from chessengine.lookup_tables import coords_to_pos, pos_to_coords
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
        default=4,
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
    parser.add_argument(
        '-v',
        '--verbose',
        default=False,
        type=bool,
        dest='verbose',
        help='Print debugging output to stdout',
        choices=[True, False]
    )

    args = parser.parse_args()

    socket = comms.get_socket(args.port, args.baud)

    player_side = input('Do you want to play white or black (w/b)? - ')
    while player_side not in ['w', 'b']:
        print('Enter "w" for white and "b" for black.')
        player_side = input('Do you want to play white or black (w/b)? - ')

    player_side = 'white' if player_side == 'w' else 'black'
    board_side = 'black' if player_side == 'white' else 'white'

    board = Board(board_side)
    engine = stockfishpy.Engine(args.path)
    engine.ucinewgame()
    engine.uci()
    print(engine.isready())
    
    side_to_move = 'white'
    print(board)

    if args.verbose:
        print(f'Starting game. Engine is playing {board_side}.')
    while True:
        if side_to_move == board_side:
            # Find move and make it (send it to arm)
            if args.engine == 'default':
                _, best_move = board.search_forward(args.depth)
                end_side, end_piece, end_board = board.identify_piece_at(best_move[1])
                board.move(best_move[0], best_move[1])
                if args.verbose:
                    print(f'Calculated best move using chessengine - {pos_to_coords[int(log2(best_move[0]))]} to {pos_to_coords[int(log2(best_move[1]))]}')
                capture = end_side is not None
                
                if args.verbose:
                    print('Sending move to arm')
                comms.send_move_to_arm(socket, (best_move[0], best_move[1]), capture)
            else:
                # Stockfish always makes the best moves
                best_move = engine.bestmove()['bestmove']
                engine.setposition([best_move])
                if args.verbose:
                    print('Calculated best move using stockfish')
                
                # Also track moves on chessengine's board to detect captures
                start = coords_to_pos[best_move[0:2].upper()]
                end = coords_to_pos[best_move[2:].upper()]
                end_side, end_piece, end_board = board.identify_piece_at(best_move[1])
                board.move(start, end)
                capture = end_side is not None

                if args.verbose:
                    print('Sending move to arm')
                comms.send_move_to_arm(socket, (best_move[0:2], best_move[2:]), capture)
        else:
            # Read move to make from serial port
            if args.verbose:
                print('Waiting for move from arm')
            start, end = comms.get_move_from_arm(socket)
            if args.verbose:
                print(f'Received move from arm - {start} to {end}')
                print(start, end, coords_to_pos[start.upper()], coords_to_pos[end.upper()])
            board.move_raw(coords_to_pos[start.upper()], coords_to_pos[end.upper()])
            engine.setposition([start+end])
            
        side_to_move = 'white' if side_to_move == 'black' else 'black'


if __name__ == '__main__':
    main()
