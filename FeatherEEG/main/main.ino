#define F_CPU 8000000UL


#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <SoftwareSerial.h>
#include "ST7735.h"
#include "LCD_GFX.h"
#include "TFT_Grapher.h"


#define TX_PIN 1  // Connect this to ESP32 RX pin
#define RX_PIN 0  // Connect this to ESP32 TX pin

// Create SoftwareSerial instance
SoftwareSerial mySerial (RX_PIN, TX_PIN);

// Global graph instance
Graph myGraph;

volatile uint16_t test_data = 0;
unsigned long lastTransmitTime = 0;
const unsigned long transmitInterval = 100; // Send data every 100ms

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

// Send data to ESP32
void sendDataToESP32(uint16_t data) {
    mySerial.println(data);
}

// ADC conversion complete ISR
ISR(ADC_vect) {
    uint16_t result = test_data;  // Read ADC result
    
    // Add new data point to graph
//    Graph_addDataPoint(&myGraph, result);
    
    // Start a new conversion immediately
    ADC_start(0);  // Continue sampling from the same channel
}

void setup() {
  // Initialize display
  lcd_init();
  LCD_setScreen(BLACK);

  pinMode(RX_PIN, INPUT);
  pinMode(TX_PIN, OUTPUT);
    
  // Initialize Serial for debugging
  Serial.begin(9600);
  
  // Initialize SoftwareSerial for ESP32 communication
  mySerial.begin(9600);
    
  // Initialize ADC
  ADC_init();
    
  // Initialize graph
  Graph_init(&myGraph, GREEN, BLACK, WHITE);
  Graph_setTitle(&myGraph, "EEG Reading (mV)");
  Graph_drawAxes(&myGraph);
  Graph_enableGrid(&myGraph, 1, BLUE);
  Graph_setScale(&myGraph, 0, 1023);
//  Graph_enableAutoScale(&myGraph, 1);
    
  // Start ADC conversion
  ADC_start(0);  // Start reading from channel 0
    
  // Enable global interrupts
  sei();
}

void loop() {
    test_data += 50;
    if (test_data > 500) {
        test_data = 0;
    }
        
    // Add some delay to control refresh rate
    Graph_addDataPoint(&myGraph, test_data);
        
    // Send data to ESP32 at regular intervals
    unsigned long currentTime = millis();
    if (currentTime - lastTransmitTime >= transmitInterval) {
        Serial.println(String(test_data));
        sendDataToESP32(test_data);
        lastTransmitTime = currentTime;
    }
        
    _delay_ms(100);
}
