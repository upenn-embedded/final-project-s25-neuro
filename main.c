#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

int main(void) {
    // Set LED pin (Pin 13) as an output
    DDRB |= (1 << PB5); // Pin 13 on Feather 328p is PB5

    while (1) {
        // Turn the LED on
        PORTB |= (1 << PB5);
        _delay_ms(1000);

        // Turn the LED off
        PORTB &= ~(1 << PB5);
        _delay_ms(1000);
    }

    return 0;
}
