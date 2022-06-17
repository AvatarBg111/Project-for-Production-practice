# Project-for-Production-practice
###### The project represents a system which makes communication between an embedded microcontroler ESP32 and a C programe, ran on a computer. The 2 devices are in the same local network and their communication is built on websockets. The ESP is a webserver and the computer programe is a webclient. The computer's programe has an interface for controlling a mechanical arm, connected to the ESP, and visualizing telemetry from the microcontroler. A part of the computer's programe receives an image from an IR camera on the ESP. It compresses it and then saves it in a given location.
*All the data which is sent through the connection is encrypted
