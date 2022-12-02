#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"
#include "led_driver.h"

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// LED color constants
uint32_t SNAKE = 0x0000FF;
#define FRUIT 0x00FF00;
#define BLANK 0x000000;


// Holds a pre-computed sine wave
int BUFFER_SIZE = 256;
uint32_t color_buffer[256] = {0};
uint16_t sample_array[6168] ={0}; //256*24+24 = 6184





void pwm_init(void) {
  // Initialize the PWM
  // EDGE_P13 is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 500 kHz, count mode to Up, and load mode to Common
  // The Countertop value doesn't matter for now. We'll set it in play_tone()
  // TODO
  nrfx_pwm_config_t cnfg = {
			    .output_pins = {EDGE_P13, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED},
			    .irq_priority = 1,
			    .base_clock = NRF_PWM_CLK_8MHz,
			    .count_mode = NRF_PWM_MODE_UP,
			    .top_value = 20,
			    .load_mode = NRF_PWM_LOAD_COMMON,
			    .step_mode = NRF_PWM_STEP_AUTO
  };
  nrfx_pwm_init(&PWM_INST, &cnfg, NULL);

}

void display_array(int* arr) {
  
  for (int i = 0; i < 32; i++) {
    for (int j=0; j < 8; j++) {
      //printf("%d",arr[i*8+j]);
      if (arr[i*8 + j] == 1) {
	color_buffer[i*8 + j] = SNAKE;
      }
      else if (arr[i*8 + j] == 2) {
	color_buffer[i*8 + j] = FRUIT;
      }
      else {
	color_buffer[i*8 + j] = BLANK;
      }
    }
  }
  printf("%x\n", color_buffer[0]);

  //printf("The non zero colors are:");

  for (int ii=0; ii<256; ii++){
    uint32_t curr_color = color_buffer[ii];
    for(int jj=0; jj<24; jj++){
      // process bit jj of curr_color into the sample array
      uint32_t color_bit=(curr_color>>jj)&1;
     
      if(color_bit==0){
        //set to low bit
        sample_array[(ii+1)*24 + jj]= (1<<15) | 3; //0.4 * 20 / 1.25 = 6.4
      }
      else{
        //set bit
        sample_array[(ii+1)*24 + jj]= (1<<15) | 7; //0.85 * 20 / 1.25 = 13.6     	
      }
    }
  }
  //x8003
  //x8003
  //x8007
  //0
  // Create the pwm sequence (nrf_pwm_sequence_t) using the samples
  // Do not make another buffer for this. You can reuse the sample buffer
  
  // Sequence structure for configuring DMA
  nrf_pwm_sequence_t pwm_sequence = {
    .values.p_common = sample_array,
    .length = 6168,
    .repeats = 0,
    .end_delay = 0,
  };

  // Start playback of the samples
  // You will need to pass in a flag to loop the sound
  // The playback count here is the number of times the entire buffer will repeat
  //    (which doesn't matter if you set the loop flag)
  // TODO
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 1, NRFX_PWM_FLAG_STOP);
}


void timer_init(void){
  //set to 32 bit timer
  NRF_TIMER4->BITMODE=3;

  //set to 16 MHz clock
  NRF_TIMER4->PRESCALER=0;

  //enable interrupts
  NRF_TIMER4->CC[0]=0;
  NRF_TIMER4->INTENSET=1<<TIMER_INTENSET_COMPARE0_Pos;

  //enable interrupts in the NVIC
  NVIC_ClearPendingIRQ(TIMER4_IRQn);
  NVIC_SetPriority(TIMER4_IRQn,7);
  NVIC_EnableIRQ(TIMER4_IRQn);

}
