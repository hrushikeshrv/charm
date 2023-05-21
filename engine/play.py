import argparse
from functools import wraps
import sys


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
            print('Exiting')
            sys.exit(1)
    return wrapper


@handle_exit
def main():
    parser = argparse.ArgumentParser(prog='Woozy', description='A chess-playing robotic arm')
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
        choices=range(1, 10),
        default=5,
        help='Set the search depth.',
        dest='depth'
    )

    args = parser.parse_args()
    if args.engine == 'default':
        pass
    else:
        pass


if __name__ == '__main__':
    main()
