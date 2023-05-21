import argparse
from functools import wraps
import sys

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
        choices=range(1, 11),
        default=5,
        type=int,
        help='Set the search depth. Can be between 1 and 10 (inclusive). Recommended depth for default engine is 4 or 5.',
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
    if args.engine == 'default':
        pass
    else:
        engine = stockfishpy.Engine(args.path)
        print(engine.isready())


if __name__ == '__main__':
    main()
