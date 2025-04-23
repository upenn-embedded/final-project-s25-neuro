/*
 * i2c.c
 * Basic I2C (TWI) driver for ATmega328PB using TWI0 interface
 */

 #include <avr/io.h>
 #include "i2c.h"
 
 void I2C_init() {
     TWSR0 = 0x00;           // Prescaler = 1
     TWBR0 = 0x48;           // SCL frequency = F_CPU / (16 + 2 * TWBR0) = ~100kHz
     TWCR0 = (1 << TWEN);   // Enable TWI
 }
 
 void I2C_start() {
     TWCR0 = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
     while (!(TWCR0 & (1 << TWINT)));
 }
 
 void I2C_repStart() {
     I2C_start();
 }
 
 void I2C_stop() {
     TWCR0 = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
 }
 
 void I2C_writeBegin(uint8_t addr) {
     I2C_start();
     TWDR0 = addr << 1; // Write mode
     TWCR0 = (1 << TWINT) | (1 << TWEN);
     while (!(TWCR0 & (1 << TWINT)));
 }
 
 void I2C_readBegin(uint8_t addr) {
     I2C_start();
     TWDR0 = (addr << 1) | 1; // Read mode
     TWCR0 = (1 << TWINT) | (1 << TWEN);
     while (!(TWCR0 & (1 << TWINT)));
 }
 
 void I2C_write(uint8_t data) {
     TWDR0 = data;
     TWCR0 = (1 << TWINT) | (1 << TWEN);
     while (!(TWCR0 & (1 << TWINT)));
 }
 
 uint8_t I2C_readAck() {
     TWCR0 = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
     while (!(TWCR0 & (1 << TWINT)));
     return TWDR0;
 }
 
 uint8_t I2C_readNack() {
     TWCR0 = (1 << TWINT) | (1 << TWEN);
     while (!(TWCR0 & (1 << TWINT)));
     return TWDR0;
 }
 
 void I2C_writeRegister(uint8_t addr, uint8_t data, uint8_t reg) {
     I2C_writeBegin(addr);
     I2C_write(reg);
     I2C_write(data);
     I2C_stop();
 }
 
 void I2C_readRegister(uint8_t addr, uint8_t* data_addr, uint8_t reg) {
     I2C_writeBegin(addr);
     I2C_write(reg);
     I2C_readBegin(addr);
     *data_addr = I2C_readNack();
     I2C_stop();
 }
 
 void I2C_writeStream(uint8_t* data, int len) {
     for (int i = 0; i < len; i++) {
         I2C_write(data[i]);
     }
 }
 
 void I2C_readStream(uint8_t* data_addr, int len) {
     for (int i = 0; i < len - 1; i++) {
         data_addr[i] = I2C_readAck();
     }
     data_addr[len - 1] = I2C_readNack();
 }
 
 void I2C_writeCompleteStream(uint8_t *dataArrPtr, uint8_t *addrArrPtr, int len, uint8_t addr) {
     I2C_writeBegin(addr);
     for (int i = 0; i < len; i++) {
         I2C_write(addrArrPtr[i]);
         I2C_write(dataArrPtr[i]);
     }
     I2C_stop();
 }
 
 void I2C_readCompleteStream(uint8_t* dataArrPtr, uint8_t addr, uint8_t reg, int len) {
     I2C_writeBegin(addr);
     I2C_write(reg);
     I2C_readBegin(addr);
     I2C_readStream(dataArrPtr, len);
     I2C_stop();
 }
 
 void ERROR() {
     // Optional: implement error handler (e.g., flash LED, reset, etc.)
 }