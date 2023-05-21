"""
Communication module for communicating between the Arduino
controller of the arm connected to a Serial port and the engine.
"""

import serial


COM_PORT = 'COM6'
BAUD_RATE = 9600


socket = serial.Serial(COM_PORT, BAUD_RATE)
