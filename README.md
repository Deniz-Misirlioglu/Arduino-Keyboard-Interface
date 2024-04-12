# Arduino-Keyboard-Interface

This project was created to interface a PS/2 keyboard with an arduino. With this library, users are able to connect a PS/2 keyboard to an Arduino Microcontroller and obtain the value at each individual key press. The PS/2 keyboard consists of a 4 pin layout {GND, 5V, Clock, Data}. When a key is pressed and released, reading 11 bits from the data pin determines which specific key was pressed. In this library, the value for all basic keys on a keyboard where mapped to a char value.

## **Interrupt Pin Variables**

* PCICR |= (1 << PCIE1);    // Enable PCIE1 (Pin Change Interrupt Enable 1)
*  PCMSK1 |= (1 << PCINT12); // Enable PCINT12 (corresponding to A4)

## Obtaining Data

Data was obtained by looping through the 11 transmitted bits on keypress and reading signal at low clock pulse.
Data was stored in a val variable.

```
uint16_t val = 0;

for (int i = 0; i < 11; i++)
    {
        while (bit_is_set(PINC, 4));       // Wait for PINC4 to be high
        val |= (bit_is_set(PINC, 3) << i); // Read PINC3 and store in 'val'
        while (bit_is_clear(PINC, 4));
    }
```

## Mapping Data

By passing the value into a switch statement, 2 flags are checked that determine if the user was holding the shift key down, and when a key was pressed and released. 
The value is then passed into the mapKeyCodeToChar() function with the shiftIsActive flag.
```
switch (val)
    {
    case 18:
    case 12576:
        //CASE TO DETERMINE IF shiftIsActive
    case 16128:
        //CASE TO DETERMINE IF A KEY HAS BEEN PRESSED AND RELEASED

    default: // any other key
        if (nextIsReleased == 0)
        {

            char asii = mapKeyCodeToChar(val, shiftIsActive);
        }
```

## Mapping Switch Statement

When the mapKeyCodeToChar function is called the value of the key is passed into another switch statement and returns the specific char.
The ternary is used with the shiftIsActive flag to determine if the key was pressed while holding shift.


``` switch (keyCode)
    {
    case 8848:
        character = shiftIsActive ? ' ' : ' ';
        break;

    case 8640:
        character = shiftIsActive ? 'A' : 'a';
        break;
    ...
    ...

return character;
```
