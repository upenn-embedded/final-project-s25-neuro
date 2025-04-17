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
 
 #define LED_PIN PD4
 #define BUZZER_PIN PD1
 #define LED_LENGTH 5
 #define BUZZER_LENGTH 10
 
 #define MAX_SCORE 3
 #define MAX_ROUND 5
 
 #define PADDLE_WIDTH 2
 #define PADDLE_HEIGHT 25
 #define PLAYER_ONE_SPEED 3
 #define PLAYER_TWO_SPEED 3
 #define BALL_RADIUS 2
 
 volatile uint8_t player_one_y = 50;
 volatile uint8_t player_two_y = 50;
 
 volatile uint8_t ball_vx = 4;
 volatile uint8_t ball_vy = 4;
 volatile uint8_t ball_x = LCD_WIDTH / 2;
 volatile uint8_t ball_y = LCD_HEIGHT / 2;
 
 int player_one_score = 0;
 int player_two_score = 0;
 
 int round_no = 1;
 int player_one_rounds_won = 0;
 int player_two_rounds_won = 0;
 
 volatile uint8_t buzzer_countdown = 0;
 volatile uint8_t led_countdown = 0;
 volatile uint8_t motor_speed = 0;
  
  /**************************************************************************//**
 * @fn			void redraw_player_one(uint8_t y)
 * @brief		redraws player 1 with updated y-coordinate
 * @note         only takes y-coordinate as parameter because x is set to 0
 *****************************************************************************/
  void redraw_player_one(uint8_t y) {
      if (y < 0 || y + PADDLE_HEIGHT > LCD_HEIGHT) {  // check bounds of display
          return; 
      }
      // clear player one
      LCD_drawBlock(0, player_one_y, PADDLE_WIDTH, player_one_y + PADDLE_HEIGHT, 0);
      // redraw player one
      player_one_y = y;
      LCD_drawBlock(0, player_one_y, PADDLE_WIDTH, player_one_y + PADDLE_HEIGHT, 0xffff);
  }
  
   /**************************************************************************//**
 * @fn			void redraw_player_two(uint8_t y)
 * @brief		redraws player 2 with updated y-coordinate
 * @note         only takes y-coordinate as parameter because x is set to 0
 *****************************************************************************/
  void redraw_player_two(uint8_t y) {
      if (y < 0 || y + PADDLE_HEIGHT > LCD_HEIGHT) {  // check bounds of display
          return; 
      }
      // clear player two
      LCD_drawBlock(LCD_WIDTH - PADDLE_WIDTH - 1, player_two_y, LCD_WIDTH, player_two_y + PADDLE_HEIGHT, 0);
      // redraw player two
      player_two_y = y;
      LCD_drawBlock(LCD_WIDTH - PADDLE_WIDTH - 1, player_two_y, LCD_WIDTH, player_two_y + PADDLE_HEIGHT, 0xffff);
  }
  
  void redraw_ball(uint8_t x, uint8_t y) {
      // clear ball
      LCD_drawCircle(ball_x, ball_y, BALL_RADIUS, 0);
      // redraw ball
      ball_x = x;
      ball_y = y;
      LCD_drawCircle(ball_x, ball_y, BALL_RADIUS, 0xffff);
  }
  
  void reset_ball() {
      // clear ball
      LCD_drawCircle(ball_x, ball_y, BALL_RADIUS, 0);
      // reset position and direction
      ball_vx = 2;
      ball_vy = -ball_vy;  // don't hard reset vy for illusion of random y direction
      ball_x = LCD_WIDTH / 2;
      ball_y = LCD_HEIGHT / 2;
  }
  
  void reset_score() {
      printf("resetting score!\n");
      player_one_score = 0;
      player_two_score = 0;
  }
  
  void reset_rounds() {
      player_one_rounds_won = 0;
      player_two_rounds_won = 0;
      round_no = 1;
  }
  
  void redraw_scoreboard() {
      // draw score
      char score[6];
      score[0] = '0' + player_one_score;
      score[1] = ' ';
      score[2] = '-';
      score[3] = ' ';
      score[4] = '0' + player_two_score;
      score[5] = '\0';
      LCD_drawString(66, 110, score, 0xffff, 0);
      
      // draw round no
      char round[10];  // Ensure enough space
      sprintf(round, "ROUND %d", round_no);
      LCD_drawString(62, 5, round, 0xffff, 0);
      
      
  }
  
  void spin_motor() {
      
      for (motor_speed = 60; motor_speed < 255; motor_speed += 5) {
          OCR2B = motor_speed;
          _delay_ms(20);
      }
         
      for (motor_speed = 255; motor_speed > 60; motor_speed -= 5) {
          OCR2B = motor_speed;
          _delay_ms(20);
      }
      
      OCR2B = 0;
  }
  
  int detect_collision() {
      int ball_right = ball_x + BALL_RADIUS;
      int ball_left = ball_x - BALL_RADIUS;
      int ball_top = ball_y - BALL_RADIUS;
      int ball_bottom = ball_y + BALL_RADIUS;
      
      // hits top/bottom
      if (ball_top <= 0 || ball_bottom >= LCD_HEIGHT) {
          ball_vy = -ball_vy;
          buzzer_countdown = BUZZER_LENGTH;
          printf("hit top/bottom: buzzer countdown %d\n", buzzer_countdown);
      }
      // hits player one paddle
      if (ball_bottom >= player_one_y && ball_top <= player_one_y + PADDLE_HEIGHT &&
              ball_left <= PADDLE_WIDTH) {
          ball_vx = -ball_vx;
          buzzer_countdown = BUZZER_LENGTH;
          printf("hit player one paddle: buzzer countdown %d\n", buzzer_countdown);
      }
      // hits player two paddle
      if (ball_bottom >= player_two_y && ball_top <= player_two_y + PADDLE_HEIGHT &&
              ball_right >= LCD_WIDTH - PADDLE_WIDTH) {
          ball_vx = -ball_vx;
          buzzer_countdown = BUZZER_LENGTH;
          printf("hit player two paddle: buzzer countdown %d\n", buzzer_countdown);
      }
      // gets past player 1
      if (ball_left <= 0) {
          buzzer_countdown = BUZZER_LENGTH;
          printf("past player 1: buzzer countdown %d\n", buzzer_countdown);
          led_countdown = LED_LENGTH;
          player_two_score++;
          
          if (player_two_score > MAX_SCORE) {
              spin_motor();
              reset_score();
              player_two_rounds_won++;
              round_no++;
              
              if (player_two_rounds_won > MAX_ROUND) {
                  reset_score();
                  reset_rounds();
              }
          }
          
          redraw_scoreboard();
          reset_ball();
      }
      if (ball_right >= LCD_WIDTH) {
          player_one_score++;
          led_countdown = LED_LENGTH;
          buzzer_countdown = BUZZER_LENGTH;
          printf("past player 2: buzzer countdown %d\n", buzzer_countdown);
          
          if (player_one_score > MAX_SCORE) {
              spin_motor();
              reset_score();
              player_one_rounds_won++;
              round_no++;
              
              if (player_one_rounds_won > MAX_ROUND) {
                  reset_score();
                  reset_rounds();
              }
          }
          
          redraw_scoreboard();
          reset_ball();
      }
  }
  
   void Initialize() {
       uart_init();
       
      // DISPLAY SETUP
      lcd_init();
      LCD_setScreen(0);  // set to black
      
      redraw_player_one(player_one_y);
      redraw_player_two(player_two_y);
      redraw_ball(ball_x, ball_y);
      redraw_scoreboard();
      
      // JOYSTICK SETUP
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
      
      
      // BUZZER SETUP
 //      set up output PD1
      DDRD |= (1<<DDD1);
      PORTD &= ~(1<<PORTD1);
     
 //     pre-scale timer 0
      TCCR4B &= ~(1<<CS42);
      TCCR4B |= (1<<CS41);
      TCCR4B |= (1<<CS40);
     
     // set timer mode normal
      TCCR4B &= ~(1<<WGM42);
      TCCR4A &= ~(1<<WGM41);
      TCCR4A &= ~(1<<WGM40);
 //     OCR4A = ;
     
     // enable compare interrupt
     TIMSK4 |= (1<<OCIE4A);
     
     sei();
      
      
      // LED SETUP
      DDRD |= (1 << PD2);
      PORTD |= (1 << PD2);
      
      // MOTOR SETUP
      // Set PD3 (OC2B) as output
      DDRD |= (1 << DDD3);
      
      // Set Timer2 to Fast PWM 8-bit mode
      TCCR2A |= (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);  // Fast PWM, non-inverting mode
      TCCR2B |= (1 << CS21) | (1 << CS20);  // Prescaler 32
     
      // initially off
      OCR2B = 0;
     
      // WIRELESS SETUP
      DDRC &= ~(1 << PC2);
      
      
  }
   
 ISR(TIMER4_COMPA_vect) {
     PORTD ^= (1 << PD1);  // Toggle the buzzer pin
     TCNT4 = 0;
 }
  
  /*
   * 
   */
  int main(int argc, char** argv) {
      Initialize();
      
      int p1_going_down = 1;
      while (1) {
          // check joystick ADC value and update player two position
          if (ADC > 510) {
              redraw_player_two(player_two_y - PLAYER_TWO_SPEED);
          }
          if (ADC < 500) {
              redraw_player_two(player_two_y + PLAYER_TWO_SPEED);
          }
          
          // update player one direction
          if (player_one_y + PADDLE_HEIGHT + PLAYER_ONE_SPEED > LCD_HEIGHT) {  // go up if hit bottom
              p1_going_down = 0;
          }
          if (player_one_y - PLAYER_ONE_SPEED < 0) {  // go down if hit top
              p1_going_down = 1;
          }
          
          // update player one position
          if (PORTC << PC2) {
              printf("going down\n");
              redraw_player_one(player_one_y + PLAYER_ONE_SPEED);
          } else {
              printf("going up\n");
              redraw_player_one(player_one_y - PLAYER_ONE_SPEED);
          }
          
          // update ball direction
          detect_collision();
          
          // update ball position
          redraw_ball(ball_x + ball_vx, ball_y + ball_vy);
          redraw_player_one(player_one_y);  // redraw players because ball leaves imprint...
          redraw_player_two(player_two_y);
          
          // light
          if (led_countdown > 0) {
              PORTD |= (1 << LED_PIN);
              led_countdown--;
          } else {
              PORTD &= ~(1 << LED_PIN);
          }
          
          _delay_ms(20);
      }
  }
  
  
   