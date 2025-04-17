void Initialize() {
    uart_init();
    
   // DISPLAY SETUP
   lcd_init();
   LCD_setScreen(0);  // set to black
}

void main() {
    Initialize();
}