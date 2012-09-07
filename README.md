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

The circuit can be connected as in the following 
schematic, in order that the board may measure its 
own input voltage:

![Voltmeter Schematic](https://github.com/wwaites/voltmeter/blob/master/voltmeter.png?raw=true)

Where the values of R1 and R2 must be appropriately
chosen so that the voltage delivered to A0, the first
analogue input pin is in the 0-5V range.
