/* 
 * File:   main.c
 * Author: sophiafu
 *
 * Created on April 23, 2025, 2:11 PM
 */

#define F_CPU 16000000UL
 #include <stdio.h>
 #include <stdlib.h>
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include "uart.h"
 #include <util/delay.h>

void Initialize() {
    
    uart_init();
    
    // ECG
    
    // Setup for ADC (10bit = 0-1023)
     // Clear power reduction bit for ADC
     PRR0 &= ~(1 << PRADC);
 
     // Select Vref = AVcc
     ADMUX |= (1 << REFS0);
     ADMUX &= ~(1 << REFS1);
 
     // Set the ADC clock div by 128
     // 16M/128=125kHz
     ADCSRA |= (1 << ADPS0);
     ADCSRA |= (1 << ADPS1);
     ADCSRA |= (1 << ADPS2);
 
     // Select Channel ADC0 (pin C0)
     ADMUX &= ~(1 << MUX0);
     ADMUX &= ~(1 << MUX1);
     ADMUX &= ~(1 << MUX2);
     ADMUX &= ~(1 << MUX3);
 
     ADCSRA |= (1 << ADATE); // Autotriggering of ADC
 
     // Free running mode ADTS[2:0] = 000
     ADCSRB &= ~(1 << ADTS0);
     ADCSRB &= ~(1 << ADTS1);
     ADCSRB &= ~(1 << ADTS2);
 
     // Disable digital input buffer on ADC pin
     DIDR0 |= (1 << ADC0D);
 
     // Enable ADC
     ADCSRA |= (1 << ADEN);
 
     // Start conversion
     ADCSRA |= (1 << ADSC);
     
     sei();
}

/*
 * 
 */
int main(int argc, char** argv) {
    Initialize();
    
    while(1) {
        printf("uart working\n");
        printf("ADC reading: %d\n", ADC);
        _delay_us(100);
    }
    return (EXIT_SUCCESS);
}

