#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "microbit_v2.h"
#include "nrf_twi_mngr.h"

#include "imu_driver.h"
#include "qwiic_led_driver.h"
#include "led_driver.h"
#include "snake_game_mini.h"
#include "play_sound.h"
#include "app_timer.h"

#define LED_ONE EDGE_P13
#define LED_TWO EDGE_P14


NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

APP_TIMER_DEF(game_timer);

int main(void) {
  printf("Board started!\n");

  // turn on any LEDs
  // make a timer to do it
  //  timer_init();

  
  //send pwm to data in PIN of LED board
  nrf_gpio_pin_dir_set(LED_ONE, NRF_GPIO_PIN_DIR_OUTPUT);
  
  //initialize PWM
  pwm_init();

  //display array
  /*
  int grid[32*8] = {0}; //all green
  grid[0] = 1;
  display_array(grid); //grab board grid from snake_game.c
  */
  
  // initialize imu
  init_imu(&twi_mngr_instance);
  
  // initialize small led display
  init_qwiic_led_driver(&twi_mngr_instance);

  // setup the game
  setup();
  
  // initialize app timer
  app_timer_init();
  app_timer_create(&game_timer, APP_TIMER_MODE_REPEATED, draw);
  
  app_timer_start(game_timer, 2500, NULL);
  
  int x = 0;
  int y = 0;
  /*
  for (int i = 0; i< 64; i++){
    for(int j = 0; j< 48; j++){
    	setPixel(i, j, 1);
    }
  }
  */
  
  while (1) {
    /*
    nrf_delay_ms(50);
    
    setPixel(x, y, 0);
    printf("Clearing (%i, %i)\n", x, y);
    
    display();
    y+= 1;
    
    if (y >= 48){
    	x += 1;
    	y = 0;
    }
    if(x >= 64){
    	x = 0;
    }
    */
    
    
    
    /*grid[0] = i;
    i += 1;
    if(i == 3){ i= 0;}
    display_array(grid);
    printf("Maybe displaying");
    */
    
    //imu_measurement_t result = read_tilt();
    //printf("hello %f \t %f \t %f \n", result.x, result.y, result.z);
    //printf("Maybe displaying");
    uint8_t result = read_tilt();
    printf("direction: %u \n", result);
    printf("playing\n");
    sound();

  }
}
