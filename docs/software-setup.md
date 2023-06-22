# Software Setup
To build and run this project, you will need the following software -

1. Python 3.7+
2. Arduino IDE
3. Git (optional, helps to clone the source code)
4. Stockfish executable (optional, but recommended)

The stockfish executable is needed if you want to use Stockfish to find moves for the arm instead of the default chess engine. The default chess engine is experimental, and Stockfish definitely plays better than it.

Follow these steps to set up the software needed to play a game with the arm -

## Python Setup
**Step 1** - Download Python
Download Python (version >= 3.7) from [python.org/downloads](https://python.org/downloads)

**Step 2** - Source Code
Get the source code for the arm from this project's repository by running 
```bash
git clone https://github.com/hrushikeshrv/charm.git
```
or by downloading the repository as a .zip archive from [github.com/hrushikeshrv/charm/archive/refs/heads/main.zip](https://github.com/hrushikeshrv/charm/archive/refs/heads/main.zip)

**Step 3** - Virtual Environment
Create a virtual environment for setting up the dependencies for this project. You can use any virtual environment manager. The easiest way is to use venv. Run the following command inside the repository folder you just cloned -

```bash
python -m venv venv
```