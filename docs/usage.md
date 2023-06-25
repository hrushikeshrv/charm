# Usage
Before starting a game with the arm, make sure you have completed the steps given on the [assembly](./assemble.md) page and the [software setup](./software-setup.md) page.

## Starting A Game
**Step 1** - Connect Arduino  
Connect your Arduino to your computer.

**Step 2** - Position Arm
Place your arm across the table from you. Make sure that you place the arm such that the axis of the motor that rotates the entire arm from its base is kept at the same distance - `armX` and `armY` - that you measured in step 2 of [software setup](./software-setup.md) under the Arduino IDE section.

**Step 3** - Run Python Code
Start a terminal session in the root directory of this project. Rememer to activate the virtual environment you created in step 4 of [software setup](./software-setup.md) under the Python Setup section.

Start the game by running the `engine/play.py` file -

```bash
python engine/play.py -c arduinoPort
```

Replace `arduinoPort` with the name of the port you have connected your Arduino to. For windows, this port is usually named `COMx`, where x is a number. For Linux/MacOS, this port is usually named `/dev/ttyUSBx`, where x is a number.

For a complete description of the options available to you when running this script, see the [play.py doc](#play-script-options). You can also run 

```bash
python engine/play.py --help
```

## Play Script Options
The Python file `engine/play.py` is used to start a game with the arm. You have the following options you can pass the script to customize behaviour -

|Option|Type|Default|Required|Description|
|------|----|-------|--------|-----------|
|`-c`, `--port`|`str`|_not set_|Yes|Set the port name that the Arduino is connected to|
|`-e`, `--engine`|`str`|`"default"`|No|The chess engine to use to find moves. Can be `"default"` for using `chessengine`, or `"stockfish"` for use Stockfish.|
|`-d`, `--depth`|`int`|`4`|No|Set the search depth. Can be an integer between 1 and 15 (inclusive). The recommended search depth while using the default engine is <= 5.
|`-p`, `--path`|`str`|`"stockfish/stockfish.exe"`|No|Set the path to the Stockfish executable.|
|`-b`, `--baud-rate`|`int`|`9600`|No|Set the baud rate for communication with the Arduino. You will only need to change this option if you modify the baud rate in the `controller/controller.ino` sketch.|
|`-v`, `--verbose`|`bool`|`False`|No|Print verbose debugging output to stdout.|

You can also pass the `--help` flag to the script to print this information.