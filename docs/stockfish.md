# Stockfish
Instead of using the `chessengine` python module for searching for moves to make you can make the arm use Stockfish to find moves to make instead.

To use Stockfish, you will need to download the Stockfish executable on your machine. You can find the latest Stockfish executable on the [official Stockfish website](https://stockfishchess.org/download/). Once you have the Stockfish executable, copy the path to it. It's usually convenient to just keep the executable in the root directory of this project, but any location is fine, as long as you can access it.

You can then run the `engine/play.py` file normally, passing the `-p` option with the path to the Stockfish executable and the `-e` option as `stockfish`. The execution command is shown below -


```bash
python engine/play.py -c arduinoPort -d 12 -p path/to/stockfish -e stockfish
```

Replace `arduinoPort` with the name of the port to which your Arduino connects to your PC. For windows, this port is usually named `COMx`, where x is a number. For Linux/MacOS, this port is usually named `/dev/ttyUSBx`, where x is a number.

If you run into any errors while trying to use Stockfish with the arm, please [open an issue](https://github.com/hrushikeshrv/charm/issues).

For a detailed description of all of the options used in the above command, as well as more options that are available to you, take a look at the [usage instructions](./usage.md#script-options).