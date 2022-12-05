#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "nrf_delay.h"
#include "microbit_v2.h"
#include "nrf_twi_mngr.h"

#include "imu_driver.h"
#include "qwiic_led_driver.h"
#include "led_driver.h"
#include "snake_game_mini.h"
#include "play_sound.h"
#include "app_timer.h"
#include "gpio.h"

#define LED_ONE EDGE_P13
#define LED_TWO EDGE_P14


NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

APP_TIMER_DEF(game_timer);

int main(void) {
  printf("Board started!\n");
  
  //send pwm to data in PIN of LED board
  nrf_gpio_pin_dir_set(LED_ONE, NRF_GPIO_PIN_DIR_OUTPUT);
  
  // initialize the buttons
  
  gpio_config(14,0);
  gpio_config(23,0);
  
  //initialize PWM
  //pwm_init();
  
  /*
  nrf_delay_ms(2000);

  //display array
  
  int grid[32*8] = {0}; //all blank
  
  for(int i = 0; i<32*8; i++){
    grid[i] = 2;
  }
  display_array(grid); //grab board grid from snake_game.c
  printf("Color should be on");
  */

  // initialize imu
  init_imu(&twi_mngr_instance);
  
  // initialize small led display
  init_qwiic_led_driver(&twi_mngr_instance);
  
  // initialize sound
  sound_pwm_init();

  // setup the game
  //setup();
  
  // initialize app timer
  app_timer_init();
  app_timer_create(&game_timer, APP_TIMER_MODE_REPEATED, draw);
  
  int x = 0;
  int y = 0;
  /*
  for (int i = 0; i< 64; i++){
    for(int j = 0; j< 48; j++){
    	setPixel(i, j, 1);
    }
  }
  */
  //int i = 0;
  
  //soundcheck();
  
  while (1) {
    //i += 1;
  	
  //setup the game upon button A pressed
  //Button A is P0.14 and active low
  //Button B is P0.23 and active low
  
    nrf_delay_ms(100);
    
    if(!gpio_read(23)){
      printf("Button B was pressed\n");
      
      setup(game_timer);
  
      app_timer_start(game_timer, 1250, NULL);
    }
    if(!gpio_read(14)){
      printf("Button A was pressed\n");
    }
    
    
    /*
    grid[0] = i % 2;
    grid[16] = i % 2;
    //printf("Color %i\n", i%2);
    
    display_array(grid);
    */
    
  }
}
