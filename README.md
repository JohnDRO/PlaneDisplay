# PlaneDisplay

The goal of this project is to display the rotation of a virtual plane in a game using step motors. The current game that I use is GTA San Andreas.

## Repository


Arduino/  -> Contains the code that runs on the Arduino. It receives order from the UART and then drive the step motor.
GTASA/    -> Contrains the code that read and compute the plane angle.

## Libraries

I use https://github.com/manashmndl/SerialPort library in order to communicate with the Arduino UNO.
