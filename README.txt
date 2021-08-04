Documentation for Image Recognition System
------------------------------------------
Files found at: https://github.com/pdanPOLY/Image-Recognition

Summary
-------
This system uses a 3x3 Resistive Memory Array of potentiometers (in series with diodes) to replicate
a crossbar array of memristors to encode a binary image into high and low resistance 
states, which can be used for image recognition. The connections involve: Raspberry Pi-to-Display,
which can be achieved via Ethernet cable to connect to a laptop screen, or an HDMI cable to a screen
accepting HDMI input; USB Connection between Raspberry Pi and Arduino; MicroUSB power supply connection
to the Raspberry Pi; various connections to get the PWM-to-DAC board connected to the crossbar
array for stabilized voltage outputs.

Tools & Technologies
--------------------
ELEGOO UNO R3 - Arduino-based microcontroller
LTC2645 Demo Board - PWM-to-DAC converter
9V Battery - External Power Supply
Fan Blade and Motor - Application of Image Recognition
100k Ohms Potentiometers (9) - Manually programmable resistors
Yellow-Green LEDs (9) - LEDs (diodes) for current control
1k Ohms Resistors (3) - Reference resistors for measuring voltage and current through at the bottom of each column
Raspberry Pi 3B - Mini-computer used for image capture and processing
Arducam 5MP Camera (X000VGJ8BL) - Camera Module connected to Pi for capturing images
Various cables and wires - Used to make connections within the system
Laptop/HDMI Input Screen - Used to display Raspberry Pi screen
Arduino IDE - Coding software for Arduino-based microcontrollers
Python - All purpose coding software used to connect image capture and processing
Visual Studio Code - Text editor that's useful for convenient coding

Arduino Code Functionality
--------------------------
Analog and Digital Pins:
A0, A1, A2 for reading voltage drops over reference resistor in the three columns
Digital 5, 6, 9 for PWM outputs to the IN A, B, C inputs of the PWM-to-DAC board, which connects VOUT A, B, C to the three rows
Digital 10, 11, 12 for outputs to the IC controlling the motor supplied by an external power supply

Main loop:
-Checks for serial input with binary code 
-Formats binary code into a 3x3 array to determine which voltages should be applied to which cells
-Computes all the average currents through each cell, and then sums the columns and checks if they are all within
 the correct range for a match
-The decided values were LRS-17.3k Ohms-2.22V and HRS-100k Ohms-4.71V, and the column current
 match range was 54uA-78uA
-Reference resistor at the bottom of each column - 1k Ohms so that it would have little influence on the overall current
-If there was a match, the motor would turn on and the fan would spin counterclockwise
 Otherwise, the motor would be off

Python Code Functionality
-------------------------
-Imports and Installations:
	Download Python, then type these commands in the command prompt:
		import cv2 -- pip install opencv-contrib-python
		import serial -- pip install pyserial
		import time
		from PIL import Image -- pip install Pillow
		import numpy as np -- pip install numpy
		(If on Raspberry Pi, need pip install picamera)
 NOTE: Everything should be installed properly on the Raspberry Pi's SD Card, but 
 to do it all over again, the keyword "sudo" must be placed in front of all the installation
 commands to work on the Pi's OS

-Image Capture:
	Using the Raspberry Pi, use PiCamera to capture the image
	Using laptop webcam, use OpenCV camera to capture the image
-Image Processing:
	Perform a binary threshold conversion to make the image black & white,
	then process it into the binary code necessary to be sent to the Arduino
-Serial Communication:
	Use pySerial's Serial object to open up a COM port connection, and then write the 
	necessary values over to the Arduino
	Once the Arduino does all the current calculations, it sends it all back to python
	serially to be displayed to the user
	When the code ends, the serial port is closed 

Results
-------
The system worked very well, being fully accurate except in a few cases where there were minor hardware
disruptions leading to incorrect current readings for specific cells. It would be cool to see this system
replicated onto an actual memristor chip so that it could take input images and automatically encode them into
memory, and then use that memory to follow up and compare other images and see if there's a match.