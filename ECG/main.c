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

// MPU6050 register addresses
#define PWR_MGMT_1 0x6B
#define ACCEL_XOUT_H 0x3B

// Global graph instance
Graph myGraph;

void mpu6050_read_accel(int16_t* ax, int16_t* ay, int16_t* az) {
    uint8_t data[6];
    I2C_readCompleteStream(data, MPU6050_ADDR, ACCEL_XOUT_H, 6);

    *ax = (int16_t)(data[0] << 8 | data[1]);
    *ay = (int16_t)(data[2] << 8 | data[3]);
    *az = (int16_t)(data[4] << 8 | data[5]);
}

#define SAMPLE_COUNT 50

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

void classify_activity(float stddev) {
    if (stddev < 500) {
        printf("Activity Level: LOW\n");
    } else if (stddev < 2000) {
        printf("Activity Level: MODERATE\n");
    } else {
        printf("Activity Level: HIGH\n");
    }
}

void Initialize() {
    
    uart_init();
    
    // IMU
    
    I2C_init();
    I2C_writeRegister(MPU6050_ADDR, 0x00, PWR_MGMT_1); // Wake up MPU6050
    _delay_ms(100);
    
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
    Graph_setScale(&myGraph, -50, 1023);
    //  Graph_enableAutoScale(&myGraph, 1);
     
    sei();
}

/*
 * 
 */
int main(int argc, char** argv) {
    Initialize();
    
    float accel_mags[SAMPLE_COUNT];
    
    while(1) {
//        printf("ADC reading: %d\n", ADC);
//        Graph_addDataPoint(&myGraph, ADC);
//        _delay_ms(10);
        
        for (int i = 0; i < SAMPLE_COUNT; i++) {
            int ax, ay, az;
            mpu6050_read_accel(&ax, &ay, &az);
//            printf("Computing man: %d, %d, %d\n", ax, ay, az);
            int x2 = ax * ax;
            printf("x2 = %d\n", x2);
            int sumsq = (ax * ax) + (ay * ay) + (az * az);
//            printf("Sum = %d\n", sumsq);
            accel_mags[i] = sqrt((ax * ax) + (ay * ay) + (az * az));
//            printf("Adding mag: %f\n", accel_mags[i]);
            _delay_ms(20); // ~50 Hz
        }

        float stddev = compute_stddev(accel_mags, SAMPLE_COUNT);
        printf("Accel StdDev: %.2f\n", stddev);
        classify_activity(stddev);
    }
    return (EXIT_SUCCESS);
}

