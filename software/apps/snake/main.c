#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "microbit_v2.h"

#include "imu_driver.h"
#include "led_driver.h"
#include "snake_game.h"

#define LED_ONE EDGE_P13
#define LED_TWO EDGE_P14

int main(void) {
  printf("Board started!\n");

  // turn on any LEDs
  // make a timer to do it
  //  timer_init();

  
  //send pwm to data in PIN of LED board
  nrf_gpio_pin_dir_set(LED_ONE, NRF_GPIO_PIN_DIR_OUTPUT);
  
  //initialize PWM
  pwm_init();


  //initialize input pin

  
  //display array
  
  int grid[32][8] = {{1}}; //all green
  
  display_array(grid); //grab board grid from snake_game.c
  
  
  
  // loop forever
  while (1) {
    nrf_delay_ms(1000);
  }
}
