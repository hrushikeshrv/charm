# Software Setup
To build and run this project, you will need the following software -

1. Python 3.7+
2. Arduino IDE
3. Git (optional, helps to clone the source code)
4. Stockfish executable (optional, but recommended)

The stockfish executable is needed if you want to use Stockfish to find moves for the arm instead of the default chess engine. The default chess engine is experimental, and Stockfish definitely plays better than it. For instructions on how to use Stockfish with the arm, take a look at the [stockfish](./stockfish.md) page.

Once you have assembled the arm and the chess board accoring to the instructions given on the [assembly](./assemble.md) page, follow the below steps to set up the software needed to play a game with the arm.

## Python Setup  
**Step 1** - Download Python  
Download and install Python (version >= 3.7) from [python.org/downloads](https://python.org/downloads)

**Step 2** - Source Code  
Get the source code for the arm from this project's repository by running 
```bash
git clone https://github.com/hrushikeshrv/charm.git
```
or by downloading the repository as a .zip archive from [github.com/hrushikeshrv/charm/archive/refs/heads/main.zip](https://github.com/hrushikeshrv/charm/archive/refs/heads/main.zip)

**Step 3** - Virtual Environment  
Create a virtual environment for installing the dependencies for this project. You can use any virtual environment manager you like. The easiest way is to use venv. Run the following command inside the repository folder you just cloned -

```bash
python -m venv venv
```

**Step 4** - Activate Environment  
Before running the arm's Python code, remember to activate the virtual environment.

For Windows, activate by running -
```
venv\scripts\activate
```

For Linux/MaxOS, activate by running -
```bash
source venv/bin/activate
```

This virtual environment will need to be activated each time you open a new terminal/shell instance.

**Step 5** - Install Dependencies  
Once you have activated your virtual environment, you can install the dependencies this project needs. Run the following command in the root directory of the project -

```bash
pip install -r requirements.txt
```

That's it for the Python setup!

## Arduino IDE

**Step 1** - IDE Download  
Download the Arduino IDE from [arduino.cc](https://www.arduino.cc/en/software)

**Step 2** - Connect Arduino  
Connect your Arduino Uno to your computer. 

The Arduino code included in this repository is specific to an Arduino Uno. If you want to use another Arduino model with the arm, you will need to change the pin numbers defined in the [`controller/controller.ino`](https://github.com/hrushikeshrv/charm/tree/main/controller) file according to the pins you will be using.

**Step 3** - Install Adafruit PWM Servo Driver Library  
To use the PCA9685 driver, you will need to install the Adafruit PWM Servo Driver Library.

In the Arduino IDE, go to the Library Manager, search for Adafruit PWM Servo Driver Library, and click Install.

**Step 4** - Modify Sketch  
The Arduino code controls the arm's movement, for which it defines some constants. All pins and constants are configured as `const int`'s near the beginning of the sketch, and there are comments describing what each constant does. 

Open the [`controller/controller.ino`](https://github.com/hrushikeshrv/charm/tree/main/controller) file in the Arduino IDE and read through the comments to figure out which constants you want to change. 

You can use the [circuit schematic](https://github.com/hrushikeshrv/charm/tree/main/data/schematics) to help. 

**Step 5** - Build & Upload  
Select your Arduino board, and click the build button and then the upload button.

If everything worked correctly, you are ready to calibrate the arm. Take a look at the [calibration](./calibration.md) page to get started.