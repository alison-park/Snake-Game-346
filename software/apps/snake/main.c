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
#include "gesture_driver.h"
#include "snake_game_mini.h"
#include "play_sound.h"
#include "app_timer.h"
#include "gpio.h"

NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

APP_TIMER_DEF(game_timer);

int main(void) {
  printf("Board started!\n");
  
  // initialize the buttons
  gpio_config(14,0);
  gpio_config(23,0);

  // initialize imu
  init_imu(&twi_mngr_instance);
  
  // initialize gesture sensor
  init_gesture(&twi_mngr_instance);
  
  // initialize small led display
  init_qwiic_led_driver(&twi_mngr_instance);
  
  // initialize sound
  sound_pwm_init();
  
  // initialize app timer
  app_timer_init();
  app_timer_create(&game_timer, APP_TIMER_MODE_REPEATED, draw);
  
  
  while (1) {
    //setup the game upon button A pressed
    //Button A is P0.14 and active low
    //Button B is P0.23 and active low
  
    nrf_delay_ms(1000);
    
    if(!gpio_read(23)){
      //printf("Button B was pressed\n");
      
      setup(game_timer);
  
      app_timer_start(game_timer, 1250, NULL);
    }
    
    if(!gpio_read(14)){
      //printf("Button A was pressed\n");
      //disable gesture sensor
     toggleInvertLock();
    }
    
  }
}
