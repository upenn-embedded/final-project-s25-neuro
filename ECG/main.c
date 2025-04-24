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
#include <util/delay.h>
#include <math.h>

#include "uart.h"
#include "ASCII_LUT.h"
#include "LCD_GFX.h"
#include "ST7735.h"
#include "TFT_Grapher.h"
#include "i2c.h"

#define MPU6050_ADDR 0x68

// MPU6050 sampling
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

#define SAMPLE_COUNT 50  // number of samples in an activity classification
#define SAMPLE_RATE 50  // in ms
#define ACCEL_SCALE 64

// high activity detection
#define ACTIVITY_SAMPLE_INTERVAL_MS (SAMPLE_RATE * SAMPLE_COUNT)  // how often activity is classified
//#define WINDOW_DURATION_MS 120000        // 2 minutes
#define WINDOW_DURATION_MS 30000
#define WINDOW_SIZE (WINDOW_DURATION_MS / ACTIVITY_SAMPLE_INTERVAL_MS)  // = 600
#define HIGH_ACTIVITY_THRESHOLD (0.2 * WINDOW_SIZE)  // 20% of samples

uint8_t activity_window[WINDOW_SIZE] = {0};  // store 0/1 values
uint16_t window_index = 0;
uint16_t high_count = 0;

uint8_t high_activity_flag = 0;

// global graph instance
Graph myGraph;

void mpu6050_read_accel(int16_t* ax, int16_t* ay, int16_t* az) {
    uint8_t data[6];
    I2C_readCompleteStream(data, MPU6050_ADDR, ACCEL_XOUT_H, 6);

    int16_t ax_raw = ((int16_t)data[0] << 8) | data[1];
    int16_t ay_raw = ((int16_t)data[2] << 8) | data[3];
    int16_t az_raw = ((int16_t)data[4] << 8) | data[5];

    *ax = ax_raw;
    *ay = ay_raw;
    *az = az_raw;
}

float compute_stddev(float* data, int len) {
    float mean = 0;
    for (int i = 0; i < len; i++) {
        mean += data[i];
    }
    mean /= len;

    float variance = 0;
    for (int i = 0; i < len; i++) {
        float diff = data[i] - mean;
        variance += diff * diff;
    }
    variance /= len;

    return sqrt(variance);
}

void update_activity_window(uint8_t is_high);

void classify_activity(float stddev) {
    uint8_t is_high = 0;
    if (stddev < 750) {
        printf("Activity Level: LOW (%f)\n", stddev);
    } else if (stddev < 2000) {
        printf("Activity Level: MODERATE (%f)\n", stddev);
    } else {
        printf("Activity Level: HIGH (%f)\n", stddev);
        is_high = 1;
    }
    update_activity_window(is_high);
}

void measure_activity_intensity() {
    float accel_mags[SAMPLE_COUNT];
    
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        int16_t ax, ay, az;
        mpu6050_read_accel(&ax, &ay, &az);
        
        float axf = (float) ax;
        float ayf = (float) ay;
        float azf = (float) az;
        
        accel_mags[i] = sqrt((axf * axf) + (ayf * ayf) + (azf * azf));
        _delay_ms(SAMPLE_RATE);
    }

    float stddev = compute_stddev(accel_mags, SAMPLE_COUNT);
    classify_activity(stddev);
}

void update_activity_window(uint8_t is_high) {
    // remove oldest value
    high_count -= activity_window[window_index];

    // insert new value
    activity_window[window_index] = is_high;
    high_count += is_high;

    // increment and wrap index
    window_index = (window_index + 1) % WINDOW_SIZE;

    if (high_count >= HIGH_ACTIVITY_THRESHOLD) {
        high_activity_flag = 1;
        printf("HIGH ACTIVITY DETECTED\n");
    } else {
        high_activity_flag = 0;
    }
}

void Initialize() {
    
    uart_init();
    
    // IMU
    
//    I2C_init();
//    I2C_writeRegister(MPU6050_ADDR, 0x00, PWR_MGMT_1); // Wake up MPU6050
//    _delay_ms(100);
    
    
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
     
     
    // LCD
    // Initialize display
    lcd_init();
    LCD_setScreen(BLACK);
    
    // Initialize graph
    Graph_init(&myGraph, GREEN, BLACK, WHITE);
    Graph_setTitle(&myGraph, "ECG Reading (mV)");
    Graph_drawAxes(&myGraph);
    Graph_enableGrid(&myGraph, 1, BLUE);
    Graph_setScale(&myGraph, -50, 1000);
    //  Graph_enableAutoScale(&myGraph, 1);
     
    sei();
}

/*
 * 
 */
int main(int argc, char** argv) {
    Initialize();
    printf("Done initializing\n");
    
    while(1) {
//        measure_activity_intensity();
        
        printf("ADC reading: %d\n", ADC);
        Graph_addDataPoint(&myGraph, ADC);
        _delay_ms(5);
    }
    return (EXIT_SUCCESS);
}

