# Charm
This document describes how to work with the chess-playing arm and how to recreate it yourself. It provides links to the following resources -

- CAD (.stp) files that were used to design the parts for the arm
- [Documentation for `chessengine`](https://chessengine.readthedocs.io/en/latest), the chess engine built specifically for this project
- [Instructions on how to use stockfish](./stockfish.md) with the arm instead of `chessengine`
- Arduino code to control the motion of the arm
- Circuit schematic for the arm
- Circuit schematic for building a custom chess board that detects moves made and communicates with the Arduino
- [Usage instructions](./usage.md) for starting a game with the arm


This page of the document describes how the arm works in great detail. The goal is to completely explain the working of the arm in simple language such that anyone reading it will be able to fully recreate it using the resources listed above. If you are referring to this document to recreate this project and find any errors, improvements, or find that any part of the document need clarification, please feel free to [open an issue](https://github.com/hrushikeshrv/charm/issues).

In order to recreate this project, you will need a basic working knowledge of -
1. Python programming, running Python programs
2. Building and uploading `.ino` files to an Arduino microcontroller
3. Reading circuit schematics and thus implementing the described circuits

# How it works
The arm can be divided into three distinct components that work together to play chess.

1. The chess engine
2. The arm
3. The chess board

These three components are glued together using some Python code running on your computer (specifically, the code in the [`engine/`](https://github.com/hrushikeshrv/charm/tree/main/engine) directory) and some C++ code running on an Arduino Uno microcontroller (specifically, the code in the [`controller/`](https://github.com/hrushikeshrv/charm/tree/main/controller) directory). The Arduino Uno is connected to your computer via its standard USB cable. The result is a robotic arm that can sit across the table from you and play a complete game of chess with you without needing any input from you.

When it is the arm's turn to move, the chess engine is responsible for finding the best move to make in the current position. The engine then communicates the best found move to the Arduino Uno connected to your computer via a USB cable. The Arduino controls the movement of the arm and makes the move it received from the computer. When it is your turn to move, the custom built chessboard detects which move you made and communicates it to the Arduino. The Arduino forwards this move to the chess engine, which then comes up with a move in response, and the game loop continues.