#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_saadc.h"
#include "nrfx_pwm.h"

#include "microbit_v2.h"

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// LED color constants
#define SNAKE 0xFF0000;
#define FRUIT 0x00FF00;
#define BLANK 0x000000;


// Holds a pre-computed sine wave
#define BUFFER_SIZE 256;
uint32_t led_buffer[BUFFER_SIZE] = {0};

// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};

// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence = {
  .values.p_common = sequence_data,
  .length = 1,
  .repeats = 0,
  .end_delay = 0,
};



static void pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
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

static void display_array(int arr[32][8]) {
  nrfx_pwm_stop(&PWM_INST, true);

  for (int i = 0; i < 32; i++) {
    for (int j=0; j < 8; j++) {
      if (arr[i][j] == 1) {
	led_buffer[i*8 + j] = SNAKE;
      }
      else if (arr[i] == 2) {
	led_buffer[i*8 + j] = FRUIT;
      }
      else {
	led_buffer[i*8 + j] = BLANK;
      }
  }
				      

  // Create the pwm sequence (nrf_pwm_sequence_t) using the samples
  // Do not make another buffer for this. You can reuse the sample buffer
  // You should set a non-zero repeat value (this is how many times each _sample_ repeats)
  // TODO
  nrf_pwm_sequence_t pwm_sequence = {
  .values.p_common = led_buffer,
  .length = BUFFER_SIZE,
  .repeats = 1,
  .end_delay = 0,
};

  // Start playback of the samples
  // You will need to pass in a flag to loop the sound
  // The playback count here is the number of times the entire buffer will repeat
  //    (which doesn't matter if you set the loop flag)
  // TODO
  nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 2, NRFX_PWM_FLAG_LOOP);
}
