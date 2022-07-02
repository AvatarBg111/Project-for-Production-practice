# Project-for-Production-practice
###### The project represents a system which makes communication between an embedded microcontroler ESP32 and a C programe, ran on a computer. The 2 devices are in the same local network and their communication is built on websockets. The ESP is a webserver and the computer programe is a webclient. The C programe on the computer can receive an IR camera picture from the ESP32. When that happens it decrypts it, saves it and a compressed copy of itself. A decompressed version of the compressed copy is also saved.
*All the data which is sent through the connection is encrypted
