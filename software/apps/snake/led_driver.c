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
uint32_t SNAKE = 0x000088;
uint32_t FRUIT = 0x004400;
uint32_t BLANK = 0x000000;


// Holds a pre-computed sine wave
uint32_t color_buffer[256] = {0};
uint16_t sample_array[696] ={0}; //256*24+24*2 = 6184





void pwm_init(void) {

  nrfx_pwm_config_t cnfg = {
			    .output_pins = {EDGE_P13, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED},
			    .irq_priority = 1,
			    .base_clock = NRF_PWM_CLK_8MHz,
			    .count_mode = NRF_PWM_MODE_UP,
			    .top_value = 10,
			    .load_mode = NRF_PWM_LOAD_COMMON,
			    .step_mode = NRF_PWM_STEP_AUTO
  };
  nrfx_pwm_init(&PWM_INST, &cnfg, NULL);
  
  for (int i =0; i<buffer_size; i++){
    sample_array[i] = 0;
  }

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
  
  for (int ii=0; ii<num_leds; ii++){
    uint32_t curr_color = color_buffer[ii];
    for(int jj=0; jj<24; jj++){
      // process bit jj of curr_color into the sample array
      // GET THE BIT
      uint32_t color_bit=(curr_color>>jj)&1;
     
      if(color_bit==0){
        //set to low bit
        sample_array[(ii)*24 + jj]= (1<<15) | 3; //0.4 * 20 / 1.25 = 6.4
      }
      else{
        //set bit
        sample_array[(ii)*24 + jj]= (1<<15) | 7; //0.85 * 20 / 1.25 = 13.6     	
      }
      
      //printf("Setting %x\n", sample_array[ii*24 + jj]);
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
    .length = buffer_size,
    .repeats = 0,
    .end_delay = 0,
  };


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
