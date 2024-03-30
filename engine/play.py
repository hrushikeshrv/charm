import argparse
from functools import wraps
from math import log2
import sys
from webbrowser import get

from chessengine import Board
from chessengine.lookup_tables import coords_to_pos, pos_to_coords
from chessengine.utils import clear_lines
import comms
from utils import square_names, get_moves_made
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
    parser = argparse.ArgumentParser(prog='charm', description='A chess-playing robotic arm')
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
        default=115200,
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
    parser.add_argument(
        '-f',
        '--feedback',
        default='auto',
        help='The feedback type. If "auto", the move made by the opponent is communicated by the arm, if "manual", the move made by the opponent needs to be entered into the terminal.',
        dest='feedback',
        choices=['auto', 'manual']
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
    print('Stockfish: ', engine.isready())
    
    side_to_move = 'white'

    if args.verbose:
        print(f'Starting game. Engine is playing {board_side}.')

    print(board)
    lines_printed = 11
    while True:
        if not args.verbose:
            clear_lines(lines_printed)
        print(board)
        lines_printed = 11
        if side_to_move == board_side:
            # Find move and make it (send it to arm)
            if args.engine == 'default':
                _, best_move = board.search_forward(args.depth)
                end_side, end_piece, end_board = board.identify_piece_at(best_move[1])
                board.move(best_move[0], best_move[1])
                capture = end_side is not None

                if args.verbose:
                    print(f'Calculated best move using chessengine - {pos_to_coords[int(log2(best_move[0]))]} to {pos_to_coords[int(log2(best_move[1]))]}')
                    print('Sending move to arm')
                    lines_printed += 2

                comms.send_move_to_arm(socket, (best_move[0], best_move[1]), capture)
            else:
                # Stockfish always makes the best moves
                best_move = engine.bestmove()['bestmove']
                engine.setposition(get_moves_made(board) + [best_move])
                if args.verbose:
                    print(f'Calculated best move using stockfish - {best_move}')
                    lines_printed += 1
                
                # Also track moves on chessengine's board to detect captures
                start = 2 ** coords_to_pos[best_move[0:2].upper()]
                end = 2 ** coords_to_pos[best_move[2:].upper()]
                end_side, end_piece, end_board = board.identify_piece_at(end)
                board.move(start, end)
                capture = end_side is not None

                if args.verbose:
                    print('Sending move to arm')
                    lines_printed += 1
                comms.send_move_to_arm(socket, (best_move[0:2], best_move[2:]), capture)
        else:
            if args.verbose:
                print('Waiting for move from arm')
                lines_printed += 1
            if args.feedback == 'auto':
                # Read move to make from serial port
                start, end = comms.get_move_from_arm(socket)
            else:
                # Read move from stdin
                move = input('Enter the move made by the opponent in the format <start_square>,<end_square> - ').split(',')
                lines_printed += 1
                while (
                    move[0].lower() not in square_names 
                    or move[1].lower() not in square_names
                    or len(move) != 2
                ):
                    print(f'Could\'nt identify the move {",".join(move)}. Enter the move in the format <start_square>,<end_square>. For example: a4,a5')
                    move = input('Enter the move made by the opponent in the format <start_square>,<end_square> - ').split(',')
                    lines_printed += 2
                start = move[0].upper()
                end = move[1].upper()
            if args.verbose:
                print(f'Received move from arm - {start} to {end}')
                lines_printed += 1
            board.move_raw(2 ** coords_to_pos[start.upper()], 2 ** coords_to_pos[end.upper()])
            engine.setposition(get_moves_made(board) + [start+end])
            
        side_to_move = 'white' if side_to_move == 'black' else 'black'


if __name__ == '__main__':
    main()
