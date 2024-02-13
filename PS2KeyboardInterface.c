#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define F_CPU 16000000UL // telling controller crystal frequency (16 MHz AVR ATMega328P)
#define BAUD 9600        // Baud rate for UART communication
#define BAUD_PRESCALLER (((F_CPU / (BAUD * 16UL))) - 1)

void UART_init()
{
    UBRR0H = (uint8_t)(BAUD_PRESCALLER >> 8);
    UBRR0L = (uint8_t)(BAUD_PRESCALLER);
    UCSR0B = (1 << TXEN0);  // Enable transmitter
    UCSR0C = (3 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit, no parity
}
// Function to transmit data
void UART_transmit(unsigned char data)
{
    while (!(UCSR0A & (1 << UDRE0)))
        ;        // Wait for empty transmit buffer
    UDR0 = data; // Put data into buffer, sends the data
}

// Function to print string
void UART_print(const char *str)
{
    while (*str)
    {
        UART_transmit(*str++);
    }
}

void UART_print_uint16(uint16_t value)
{
    char buffer[6];          // Maximum 5 characters for uint16_t value and 1 for null terminator
    itoa(value, buffer, 10); // Convert uint16_t to string
    UART_print(buffer);      // Print the string
}
char ScancodeToASCII[2][128] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 94, 0, 0, 0, 0, 0, 0, 113, 49, 0, 0, 0, 122, 115, 97, 119, 50, 0, // w/o SHIFT or ALT(GR)
     0, 99, 120, 100, 101, 52, 51, 0, 0, 32, 118, 102, 116, 114, 53, 0, 0, 110, 98, 104, 103, 121, 54, 0, 0, 0, 109, 106, 117, 55, 56, 0,
     0, 44, 107, 105, 111, 48, 57, 0, 0, 46, 45, 108, 148, 112, 0, 0, 0, 0, 132, 0, 129, 96, 0, 0, 0, 0, 10, 43, 0, 35, 0, 0,
     0, 60, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 248, 0, 0, 0, 0, 0, 0, 81, 33, 0, 0, 0, 90, 83, 65, 87, 34, 0, // with SHIFT
     0, 67, 88, 68, 69, 36, 0, 0, 0, 0, 86, 70, 84, 82, 37, 0, 0, 78, 66, 72, 71, 89, 38, 0, 0, 0, 77, 74, 85, 47, 40, 0,
     0, 59, 75, 73, 79, 61, 41, 0, 0, 58, 95, 76, 153, 80, 63, 0, 0, 0, 142, 0, 154, 0, 0, 0, 0, 0, 0, 42, 0, 39, 0, 0,
     0, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void setup()
{
    cli(); // Disable interrupts globally

    // Setup PCINT1 (Pin Change Interrupt 1) for A4 on ATmega328P
    PCICR |= (1 << PCIE1);    // Enable PCIE1 (Pin Change Interrupt Enable 1)
    PCMSK1 |= (1 << PCINT12); // Enable PCINT12 (corresponding to A4)

    sei(); // Enable interrupts globally
}

void loop()
{
    // Your main code logic goes here
}

ISR(PCINT1_vect)
{
    uint16_t val = 0;

    for (int i = 0; i < 11; i++)
    {
        while (bit_is_set(PINC, 4))
            ;                              // Wait for PINC4 to be high
        val |= (bit_is_set(PINC, 3) << i); // Read PINC3 and store in 'val'
        while (bit_is_clear(PINC, 4))
            ;
    }

    UART_print_uint16(val);
    UART_print("-");
    static uint8_t shiftIsActive = 0;  // state of the SHIFT key
    static uint8_t nextIsReleased = 0; // indicating that the next key was released

    switch (val)
    {
    case 18:
    case 9999:
        shiftIsActive = !nextIsReleased;
        nextIsReleased = 0;
        break; // SHIFT LEFT, SHIFT RIGHT
    case 999:
        nextIsReleased = 1;
        break;               // key release indicator
    default:                 // any other key
        if (!nextIsReleased) // is it a 'key pressed' event?
        {

            // UART_print_uint16(val);
            // UART_print("-");
            uint16_t asciiCode = ScancodeToASCII[shiftIsActive][val & 127];

            if (asciiCode != 0) // Check if there is a valid ASCII code
            {
            }
        }
        nextIsReleased = 0;
        break;
    }
    PCIFR |= (1 << PCIF1); // clears the PCI flag 1
}

int main(void)
{
    UART_init();

    UART_print("Distance: ");
    setup();
    while (1)
    {
        loop();
    }

    return 0;
}
