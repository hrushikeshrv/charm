# Charm
This document describes how to work with the chess-playing arm and how to recreate it yourself. It provides links to the following resources -

- [CAD files](https://github.com/hrushikeshrv/charm/tree/main/data/cad) that were used to design the parts for the arm
- [Documentation for `chessengine`](https://chessengine.readthedocs.io/en/latest), the chess engine built specifically for this project
- [Instructions on how to use stockfish](./stockfish.md) with the arm instead of `chessengine`
- [Circuit schematic](https://github.com/hrushikeshrv/charm/tree/main/data/schematics) for the arm, including a custom chess board that detects moves made and communicates with the Arduino
- [Usage instructions](./usage.md) for starting a game with the arm
- [Assembly instructions](./assemble.md) that provide step-by-step instructions on how to build the arm once you have all the components
- [How it works](./working.md)


This page of the document describes how the arm works in great detail. The goal is to completely explain the working of the arm in simple language such that anyone reading it will be able to fully recreate it using the resources listed above.

In order to recreate this project, you will need a basic working knowledge of -
1. Python programming, running Python programs
2. Building and uploading `.ino` files to an Arduino microcontroller
3. Reading circuit schematics and thus implementing the described circuits
4. Using a command prompt/terminal

To recreate this project, read this document in the order given below -

1. This page
2. [Assembly/Hardware Setup](./assemble.md)
3. [Software Setup](./software-setup.md)
4. [Usage](./usage.md)
5. [Stockfish](./stockfish.md)

If you find any errors, improvements, or find that any part of the document needs clarification, please feel free to [open an issue](https://github.com/hrushikeshrv/charm/issues). This document and this project is licensed under the GPL v3.0 License, which can be found [here](https://github.com/hrushikeshrv/charm/blob/main/LICENSE).
