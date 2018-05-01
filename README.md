# PlaneDisplay

The goal of this project is to read the matrix rotation of a vehicle in a video game. Once we have those angles, I transfer this data to an Arduino UNO that drives stepper motors. Those stepper motors will display the currents angles of the vehicle.

Reading the matrix rotation is easy. I read the memory - using the ReadProcessMemory() function - of GTA San Andreas in order to obtain it. Look at main.cpp in the GTASA folder to read the code. GTA San Andreas Memory addresses are quite well known and well documented.

In order to drive the stepper motors, I use DRV8825 modules Those modules are quite simple. Each pulse on the STEP pin will drive the motor to one step. In order to choose the direction, I correctly drive the DIR pin.

## Bloc diagram

![alt text](https://i.imgur.com/3dpn9VX.png "Bloc diagram")

## Repository

./GTASA/    -> Contains the code that read and compute the plane angle.
./Arduino/  -> Contains the code that runs on the Arduino. It receives order from the UART and then drive the step motor.

## Interesting Libraries / Links

[SerialPort Library](https://github.com/manashmndl/SerialPort) : a C++ library to easily communicate with an Arduino board from Windows (using a serial link).
[GTA San Andreas Memory Addresses](https://www.gtamodding.com/wiki/Memory_Addresses_(SA)) : a description of various memory addresses. It tells us where to find the rotation matrix for instance.
[Arduino & Driver de moteur pas à pas DRV8825](http://gilles.thebault.free.fr/spip.php?article29) : basic usage and code example on how to use the DRV8825 from an Arduino UNO.

# Angle reading example

![alt text](https://i.imgur.com/8S0VcbO.png "Angle reading example")
