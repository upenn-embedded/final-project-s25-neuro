#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include "TFT_Grapher.h"

// Global graph instance
Graph myGraph;

volatile uint16_t test_data = 0;

// ADC initialization
void ADC_init() {
    // Configure ADC
    ADMUX = (1 << REFS0);  // Use AVCC as reference
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (7 << ADPS0);  // Enable ADC, enable interrupt, prescaler = 128
}

// Start a new ADC conversion
void ADC_start(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Select ADC channel
    ADCSRA |= (1 << ADSC);  // Start conversion
}

// ADC conversion complete ISR
ISR(ADC_vect) {
    uint16_t result = test_data;  // Read ADC result
    
    // Add new data point to graph
//    Graph_addDataPoint(&myGraph, result);
    
    // Start a new conversion immediately
    ADC_start(0);  // Continue sampling from the same channel
}

int main(void) {
    // Initialize display
    lcd_init();
    LCD_setScreen(BLACK);
    
    // Initialize ADC
    ADC_init();
    
    // Initialize graph
    Graph_init(&myGraph, GREEN, BLACK, WHITE);
    Graph_setTitle(&myGraph, "EEG Reading (mV)");
    Graph_drawAxes(&myGraph);
    Graph_enableGrid(&myGraph, 1, BLUE);
    Graph_setScale(&myGraph, 0, 1023);
//    Graph_enableAutoScale(&myGraph, 1);
    
    
    // Start ADC conversion
    ADC_start(0);  // Start reading from channel 0
    
    // Enable global interrupts
    sei();
    
    // Main loop
    while (1) {
        
        test_data += 50;
        if (test_data > 500) {
            test_data = 0;
        }
        // Add some delay to control refresh rate
        Graph_addDataPoint(&myGraph, test_data);
        _delay_ms(100);
    }
    
    return 0;
}