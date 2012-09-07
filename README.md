Arduino Voltmeter
=================

To build and upload to the board:

    export ARDUINODIR=/some/where/arduino-1.0.1
    export SERIALDEV=/dev/ttyACM0
    make all upload

Thereafter, the recorded voltages can be retrieved via 
HTTP from <http://10.11.12.23/> as a JSON document.

The program has a circular buffer and, by default, will
record voltages every minute for four minutes.
