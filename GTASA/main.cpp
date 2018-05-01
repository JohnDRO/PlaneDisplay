// Project : Plane Display
// B.
//
// This code read GTA San Andreas Memory.
// It then check if the player is in a vehicle.
// If he is, this code get the vehicle's angles and send it to the COM port.
//
// Useful links :
// GTA San Andreas Memory mapping : https://www.gtamodding.com/wiki/Memory_Addresses_(SA)
// COM PORT write : SerialPort library https://blog.manash.me/serial-communication-with-an-arduino-using-c-on-windows-d08710186498 | https://github.com/manashmndl/SerialPort

#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "SerialPort.h"

using namespace std;

int main() {
    // Windows stuff
    DWORD pid;
	HWND hwnd;

	// Memory pointers and data
	DWORD CPED;
	DWORD CarPointer;
	DWORD RotationMatrix;

    DWORD MemoryAddress;
	DWORD MemoryData_DWORD;
	uint8_t MemoryData_uint8;

    // vehicle angles
	float anglex, angley, anglez;
	float anglex_grad, angley_grad, anglez_grad;
	float anglex_look, angley_look, anglez_look;

	// SerialPort Library
	SerialPort *arduino = new SerialPort("\\\\.\\COM3");

	if (!arduino->isConnected()) {
        cout << "Arduino/Serial Port not connected/working :(";
        return 0;
	}

    // Let's try to find GTA:SA
    hwnd = FindWindow(NULL,"GTA:SA:MP");

	if(!hwnd) {
        cout <<"GTA:SA:MP : window not found :(!\n";
        return 0;
	}

	else {
        // Try to get the hangle on the GTA:SA:MP process.
        // In order to read and write data (ReadProcessMemory & WriteProcessMemory functions).
        GetWindowThreadProcessId(hwnd,&pid);
        HANDLE phandle = OpenProcess(PROCESS_VM_READ,0,pid);

		if(!phandle) {
            cout <<"GTA:SA:MP : could not get handle :(!\n";
            return 0;
		}

		else {
            // We can now read GTA:SA:MP process memory.
            // We are going to read the player pointer (CPed)
            MemoryAddress = 0xB6F5F0;
            ReadProcessMemory(phandle, (void*)MemoryAddress, &MemoryData_DWORD, sizeof(MemoryData_DWORD), 0);

            CPED = MemoryData_DWORD;
            // --

            while(1) {
                // We need to check the player status : is he in a car?
                // Check if player is in a car
                // Cped +0x46C = [byte] Player check:
                // * 0 = in air/water
                // * 1 = in car
                // * 2 = entering interior
                // * 3 = on foot
                MemoryAddress = CPED + 0x46C;
                ReadProcessMemory(phandle, (void*)MemoryAddress, &MemoryData_uint8, sizeof(MemoryData_uint8), 0);
                // --

                if (MemoryData_uint8 == 1) { // Player in a car?

                    // He is in a car, we need to get the CarPointer
                    // CPed +0x58C = [dword] Last or Current Driven Car (CarPointer)
                    MemoryAddress = CPED + 0x58C;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &MemoryData_DWORD, sizeof(MemoryData_DWORD), 0);

                    CarPointer = MemoryData_DWORD;
                    // --

                    // With this CarPointer, we can have access to the Rotation Matrix
                    // CPed +20 = [byte] Contains a pointer to the rotation/position matrix (84 bytes) : RotationMatrix
                    //
                    // RotationMatrix +  0 = [float] X-axis Rotation (Grad)
                    // RotationMatrix +  4 = [float] Y-axis Rotation (Grad)
                    // RotationMatrix +  8 = [float] Z-axis Rotation (Grad)
                    // RotationMatrix + 16 = [float] X-axis Rotation (Looking)
                    // RotationMatrix + 20 = [float] Y-axis Rotation (Looking)
                    // RotationMatrix + 24 = [float] Z-axis Rotation (Looking)
                    MemoryAddress = (CarPointer + 20);
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &RotationMatrix, sizeof(RotationMatrix), 0);

                    // We have the address of the RotationMatrix, we are now going to read angles.
                    // Get car rotation angles
                    MemoryAddress = RotationMatrix + 0;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &anglex_grad, sizeof(anglex_grad), 0);

                    MemoryAddress = RotationMatrix + 16;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &anglex_look, sizeof(anglex_look), 0);

                    MemoryAddress = RotationMatrix + 4;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &angley_grad, sizeof(angley_grad), 0);

                    MemoryAddress = RotationMatrix + 20;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &angley_look, sizeof(angley_look), 0);

                    MemoryAddress = RotationMatrix + 8;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &anglez_grad, sizeof(anglez_grad), 0);

                    MemoryAddress = RotationMatrix + 24;
                    ReadProcessMemory(phandle, (void*)MemoryAddress, &anglez_look, sizeof(anglez_look), 0);

                    // We have our rotation matrix angles.
                    // Let's translate them in something that we can use with the Arduino.
                    // Computing is a bit messy, and there is surely a more efficient way, sorry.
                    if (angley_look >= 0 && anglex_look >= 0) { // First Quadrant
                        if (anglex_grad >= 0) { // The vehicle is NOT upside down.
                            if (anglez_grad >= 0) angley = asin(anglez_grad) * 180.0/3.1415;
                            else angley = 360 + asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez = asin(anglez_look) * 180.0/3.1415;
                            else anglez = 360 + asin(anglez_look) * 180.0/3.1415;

                            anglex = acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else if (anglex_grad < 0) { // The vehicle is upside down.
                            if (anglez_grad >= 0) angley = 180 - asin(anglez_grad) * 180.0/3.1415;
                            else angley = 180 - asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez =  -asin(anglez_look) * 180.0/3.1415;
                            else anglez = -asin(anglez_look) * 180.0/3.1415;

                            anglex = acos(anglex_grad / cos(3.1415 - asin(anglez_grad))) * 180.0/3.1415;

                        }

                        else  { // Should not be possible, but let's be sure.
                            anglex = 0;
                            angley = 0;
                            anglez = 0;
                        }
                    }

                    else if (angley_look < 0 && anglex_look > 0) { // Second Quadrant
                       if (anglex_grad <= 0) { // The vehicle is NOT upside down.
                            if (anglez_grad >= 0) angley = asin(anglez_grad) * 180/3.1415;
                            else angley = 360 + asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez = asin(anglez_look) * 180.0/3.1415;
                            else anglez = 360 + asin(anglez_look) * 180.0/3.1415;

                            anglex = acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else if (anglex_grad > 0) { // The vehicle is upside down.
                            if (anglez_grad >= 0) angley =  180 - asin(anglez_grad) * 180.0/3.1415;
                            else angley = 180 - asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez =  -asin(anglez_look) * 180.0/3.1415;
                            else anglez = -asin(anglez_look) * 180.0/3.1415;

                            anglex = 180 - acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else  { // Should not be possible, but let's be sure.
                            anglex = 0;
                            angley = 0;
                            anglez = 0;
                        }
                    }

                    else if (angley_look < 0 && anglex_look < 0) { // Third Quadrant
                        if (anglex_grad <= 0) { // The vehicle is NOT upside down.
                            if (anglez_grad >= 0) angley = asin(anglez_grad) * 180.0/3.1415;
                            else angley = 360 + asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez = asin(anglez_look) * 180.0/3.1415;
                            else anglez = 360 + asin(anglez_look) * 180.0/3.1415;

                            anglex = 360 - acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else if (anglex_grad > 0) { // The vehicle is upside down.
                            if (anglez_grad >= 0) angley =  180 - asin(anglez_grad) * 180.0/3.1415;
                            else angley = 180 - asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez =  -asin(anglez_look) * 180.0/3.1415;
                            else anglez = -asin(anglez_look) * 180.0/3.1415;

                            anglex = 180 + acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else  { // Should not be possible, but let's be sure.
                            anglex = 0;
                            angley = 0;
                            anglez = 0;
                        }
                    }

                    else if (angley_look > 0 && anglex_look < 0) { // Fourth Quadrant
                        if (anglex_grad >= 0) { // The vehicle is NOT upside down.
                            if (anglez_grad >= 0) angley = asin(anglez_grad) * 180.0/3.1415;
                            else angley = 360 + asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez = asin(anglez_look) * 180.0/3.1415;
                            else anglez = 360 + asin(anglez_look) * 180.0/3.1415;

                            anglex = 360 - acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else if (anglex_grad < 0) { // The vehicle is upside down.
                            if (anglez_grad >= 0) angley =  180 - asin(anglez_grad) * 180.0/3.1415;
                            else angley = 180 - asin(anglez_grad) * 180.0/3.1415;

                            if (anglez_look >= 0) anglez =  -asin(anglez_look) * 180.0/3.1415;
                            else anglez = -asin(anglez_look) * 180.0/3.1415;

                            anglex = 180 + acos(anglex_grad / cos(asin(anglez_grad))) * 180.0/3.1415;
                        }

                        else  { // Should not be possible, but let's be sure.
                            anglex = 0;
                            angley = 0;
                            anglez = 0;
                        }

                    }

                    else { // Should not be possible, but let's be sure.
                        anglex = 0;
                        angley = 0;
                        anglez = 0;
                    }

                    cout << "Player is in a car.\n";
                    cout << "\t[Angle X] : " << anglex << "\n";
                    cout << "\t[Angle Y] : " << angley << "\n";
                    cout << "\t[Angle Z] : " << anglez << "\n";

                    // Debug
                    printf("[anglex_grad] %f\n", anglex_grad);
                    printf("[anglex_look] %f\n", anglex_look);
                    printf("[angley_grad] %f\n", angley_grad);
                    printf("[angley_look] %f\n", angley_look);
                    printf("[anglez_grad] %f\n", anglez_grad);
                    printf("[anglez_look] %f\n", anglez_look);
                    // --

                    // We have our angles : let's send it to the Arduino through the COM port
                    if (arduino->isConnected()){

                        // We need to format our data.
                        // The Arduino waits data similar to : "$AngleX.$AngleY.$AngleZ.\n"
                        // $AngleX is the angle. It must be a int.
                        // '.' indicates the end of data.
                        // '\n' indicates the end of line.

                        // COM port buffer, contrains the str to send.
                        char buffer [255];
                        sprintf (buffer, "%03d.%03d.%03d.\n", (int) anglex, (int) angley, (int) anglez);

                        if (arduino->writeSerialPort(buffer, 13)) cout << "SerialPort Send OK \n";
                        else cout << "SerialPort Send KO \n";
                    }

                    else {
                        cout << "Arduino/Serial Port not connected/working :(";
                        return 0;
                    }

                }

                else {
                    // Player is not in a car.
                    // Let's do nothing.

                    ;

                    cout << "Player is not in a car.\n";
                }

                // Set the update rate in ms.
                Sleep(500);
            }

		return 0;

		}
	}
}
