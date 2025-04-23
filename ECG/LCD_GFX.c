/*
 * LCD_GFX.c
 *
 * Created: 9/20/2021 6:54:25 PM
 *  Author: You
 */ 

 #include "LCD_GFX.h"
 #include "ST7735.h"
 #include <stdlib.h>
 
 /******************************************************************************
 * Local Functions
 ******************************************************************************/
 
 
 
 /******************************************************************************
 * Global Functions
 ******************************************************************************/
 
 /**************************************************************************//**
 * @fn			uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
 * @brief		Convert RGB888 value to RGB565 16-bit color data
 * @note
 *****************************************************************************/
 uint16_t rgb565(uint8_t red, uint8_t green, uint8_t blue)
 {
     return ((((31*(red+4))/255)<<11) | (((63*(green+2))/255)<<5) | ((31*(blue+4))/255));
 }
 
 /**************************************************************************//**
 * @fn			void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color)
 * @brief		Draw a single pixel of 16-bit rgb565 color to the x & y coordinate
 * @note
 *****************************************************************************/
 void LCD_drawPixel(uint8_t x, uint8_t y, uint16_t color) {
     LCD_setAddr(x,y,x,y);
     SPI_ControllerTx_16bit(color);
 }
 
 /**************************************************************************//**
 * @fn			void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor)
 * @brief		Draw a character starting at the point with foreground and background colors
 * @note
 *****************************************************************************/
 void LCD_drawChar(uint8_t x, uint8_t y, uint16_t character, uint16_t fColor, uint16_t bColor){
     uint16_t row = character - 0x20;		//Determine row of ASCII table starting at space
     int i, j;
     if ((LCD_WIDTH-x>7)&&(LCD_HEIGHT-y>7)){
         for(i=0;i<5;i++){
             uint8_t pixels = ASCII[row][i]; //Go through the list of pixels
             for(j=0;j<8;j++){
                 if ((pixels>>j)&1==1){
                     LCD_drawPixel(x+i,y+j,fColor);
                 }
                 else {
                     LCD_drawPixel(x+i,y+j,bColor);
                 }
             }
         }
     }
 }
 
 
 /******************************************************************************
 * LAB 4 TO DO. COMPLETE THE FUNCTIONS BELOW.
 * You are free to create and add any additional files, libraries, and/or
 *  helper function. All code must be authentically yours.
 ******************************************************************************/
 
 /**************************************************************************//**
 * @fn			void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius,uint16_t color)
 * @brief		Draw a colored circle of set radius at coordinates
 * @note
 *****************************************************************************/
 
 void LCD_drawCircle(uint8_t x0, uint8_t y0, uint8_t radius, uint16_t color)
 {
     // Use Bresenham's circle drawing algorithm
     int x = 0;
     int y = radius;
     int d = 3 - 2 * radius;
 
     while (x <= y) {
         // check for decision parameter
         // and correspondingly 
         // update d, y
         if (d > 0) {
             y--; 
             d = d + 4 * (x - y) + 10;
         }
         else
             d = d + 4 * x + 6;
         x++;
     
         LCD_drawPixel(x0 + x, y0 + y, color);
         LCD_drawPixel(x0 - x, y0 + y, color);
         LCD_drawPixel(x0 + x, y0 - y, color);
         LCD_drawPixel(x0 - x, y0 - y, color);
         LCD_drawPixel(x0 + y, y0 + x, color);
         LCD_drawPixel(x0 - y, y0 + x, color);
         LCD_drawPixel(x0 + y, y0 - x, color);
         LCD_drawPixel(x0 - y, y0 - x, color);
     }
 }
 
 
 /**************************************************************************//**
 * @fn			void LCD_drawLine(short x0,short y0,short x1,short y1,uint16_t c)
 * @brief		Draw a line from and to a point with a color
 * @note
 *****************************************************************************/
 void LCD_drawLine(short x0, short y0, short x1, short y1, uint16_t c) {
     int dx = abs(x1 - x0);
     int dy = abs(y1 - y0);
     int sx = (x0 < x1) ? 1 : -1;  // Direction of the x-axis
     int sy = (y0 < y1) ? 1 : -1;  // Direction of the y-axis
     int err = dx - dy;            // Error term to determine when to adjust x or y
 
     while (1) {
         LCD_drawPixel(x0, y0, c);  // Draw the pixel at the current coordinates
 
         // If the line has reached the end point, break the loop
         if (x0 == x1 && y0 == y1) break;
 
         int e2 = err * 2;  // Double the error term for comparison
         if (e2 > -dy) {
             err -= dy;  // Adjust the error and update x
             x0 += sx;
         }
         if (e2 < dx) {
             err += dx;  // Adjust the error and update y
             y0 += sy;
         }
     }
 }
 
 
 
 /**************************************************************************//**
 * @fn			void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1,uint16_t color)
 * @brief		Draw a colored block at coordinates
 * @note
 *****************************************************************************/
 void LCD_drawBlock(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
 {
     LCD_setAddr(x0, y0, x1, y1);
     for (int i = 0; i < (x1 - x0 + 1) * (y1 - y0 + 1); i++) {
         SPI_ControllerTx_16bit(color);
     }
 }
 
 /**************************************************************************//**
 * @fn			void LCD_setScreen(uint16_t color)
 * @brief		Draw the entire screen to a color
 * @note
 *****************************************************************************/
 void LCD_setScreen(uint16_t color) 
 {
 //	for (int x = 0; x <= LCD_WIDTH; x++) {
 //        for (int y = 0; y <= LCD_HEIGHT; y++) {
 //            LCD_drawPixel(x, y, color);
 //        }
 //    }
     LCD_setAddr(0, 0, LCD_WIDTH, LCD_HEIGHT);
     for (uint16_t i = 0; i <= LCD_WIDTH * LCD_HEIGHT; i++) {
         SPI_ControllerTx_16bit(color);
     }
 }
 
 /**************************************************************************//**
 * @fn			void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg)
 * @brief		Draw a string starting at the point with foreground and background colors
 * @note
 *****************************************************************************/
 void LCD_drawString(uint8_t x, uint8_t y, char* str, uint16_t fg, uint16_t bg) {
     int x_incr = x;
     
     int i = 0;
     char c = str[i];
     while(c != '\0') {
         LCD_drawChar(x_incr, y, c, fg, bg);
         
         c = str[++i];
         x_incr += 5;
     }
 }