# Arduino-Keyboard-Interface

This project was created to interface a PS/2 keyboard with an arduino. With this library, users are able to connect a PS/2 keyboard to an Arduino Microcontroller and obtain the value at each individual key press. The PS/2 keyboard consists of a 4 pin layout {GND, 5V, Clock, Data}. When a key is pressed and released, reading 11 bits from the data pin determines which specific key was pressed. In this library, the value for all basic keys on a keyboard where mapped to a char value.
