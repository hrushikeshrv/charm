# Using Stockfish
Instead of using the `chessengine` python module for searching for moves to make you can make the arm use Stockfish to find moves to make instead.

To use Stockfish, you need to download the Stockfish executable on your machine. You can find the latest Stockfish executable for download on the [official Stockfish website](https://stockfishchess.org/download/).

Once you have downloaded the executable, you can run the `engine/play.py` file normally, passing the `-p` option with the path to the Stockfish executable and the `-e` option as `stockfish`. The execution command is shown below -


For windows -

```bash
python engine/play.py -p COM6 -d 12 -p path/to/stockfish -e stockfish
```

For Linux/MacOS -

```bash
python engine/play.py -p dev/ttyUSB3 -d 12 -p path/to/stockfish -e stockfish
```

Take a look at the [usage](./usage.md) page for other options you can set on the arm.