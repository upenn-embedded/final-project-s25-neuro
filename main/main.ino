/* 
 * File:   main.c
 * Author: sophiafu
 *
 * Created on March 21, 2025, 2:55 PM
 */

 #define F_CPU 16000000UL
 #include <stdio.h>
 #include <stdlib.h>
 #include <avr/io.h>
 #include <avr/interrupt.h>
 #include <util/delay.h>
 
 #include "uart.h"
 #include "LCD_GFX.h"
 #include "ST7735.h"
 #include "ASCII_LUT.h"
 
  
   void Initialize() {
       uart_init();
       
      // DISPLAY SETUP
      lcd_init();
      LCD_setScreen(0);  // set to black

   }
  
  /*
   * 
   */
  int main(int argc, char** argv) {
      Initialize();
      
  }
  
  
   