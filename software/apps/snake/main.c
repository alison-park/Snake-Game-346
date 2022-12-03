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

#include "app_timer.h"

#define LED_ONE EDGE_P13
#define LED_TWO EDGE_P14

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
  
  // setup the game
  setup();
  
  // initialize app timer
  app_timer_init();
  app_timer_create(&game_timer, APP_TIMER_MODE_REPEATED, draw);
  
  app_timer_start(game_timer, 993, NULL);
  
  while (1) {
    
    //nrf_delay_ms(50);
    /*grid[0] = i;
    i += 1;
    if(i == 3){ i= 0;}
    display_array(grid);
    printf("Maybe displaying");
    */
    
    //display();
    
    
    //imu_measurement_t result = read_tilt();
    //printf("hello %f \t %f \t %f \n", result.x, result.y, result.z);
    //printf("Maybe displaying");
    //uint8_t result = read_tilt();
    //printf("direction: %u \n", result);
  }
}
