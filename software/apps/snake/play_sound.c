
// PWM Square wave tone app
//
// Use PWM to play a tone over the speaker using a square wave

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "nrf.h"
#include "nrf_delay.h"
#include "nrfx_pwm.h"
#include "play_sound.h"

#include "microbit_v2.h"

// PWM configuration
static const nrfx_pwm_t PWM_INST = NRFX_PWM_INSTANCE(0);

// Holds duty cycle values to trigger PWM toggle
nrf_pwm_values_common_t sequence_data[1] = {0};



// Sequence structure for configuring DMA
nrf_pwm_sequence_t pwm_sequence = {
  .values.p_common = sequence_data,
  .length = 1,
  .repeats = 0,
  .end_delay = 0,
};


void sound_pwm_init(void) {
  // Initialize the PWM
  // SPEAKER_OUT is the output pin, mark the others as NRFX_PWM_PIN_NOT_USED
  // Set the clock to 500 kHz, count mode to Up, and load mode to Common
  // The Countertop value doesn't matter for now. We'll set it in play_tone()
  // TODO
  nrfx_pwm_config_t config = {
   .output_pins = {SPEAKER_OUT, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED, NRFX_PWM_PIN_NOT_USED},
  .irq_priority = 1,
  .base_clock = NRF_PWM_CLK_500kHz,
  .count_mode = NRF_PWM_MODE_UP,
  .top_value = 0,
  .load_mode = NRF_PWM_LOAD_COMMON,
  .step_mode = NRF_PWM_STEP_AUTO,
  };
  nrfx_pwm_init(&PWM_INST, &config, NULL);
  
}

void play_tone(uint16_t frequency) {
  // Stop the PWM (and wait until its finished)
  // TODO
     nrfx_pwm_stop(&PWM_INST, true);
  // Set a countertop value based on desired tone frequency
  // You can access it as NRF_PWM0->COUNTERTOP
  // TODO
     NRF_PWM0->COUNTERTOP = 500000/frequency;
  // Modify the sequence data to be a 25% duty cycle
  // TODO
     sequence_data[0] =(500000/frequency)/2;
  // Start playback of the samples and loop indefinitely
  // TODO
     nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 10, NRFX_PWM_FLAG_LOOP);
}   


void ascend(){
  printf("I'm being called");
  play_tone(440);
  nrf_delay_ms(500);

  play_tone(554);
  nrf_delay_ms(500);

  play_tone(659);
  nrf_delay_ms(500);

  play_tone(880);
  nrf_delay_ms(500);

  nrfx_pwm_stop(&PWM_INST, true);
}


void descend(){

  play_tone(880);
  nrf_delay_ms(500);
  
  play_tone(659);
  nrf_delay_ms(500);

  play_tone(554);
  nrf_delay_ms(500);

  play_tone(440);
  nrf_delay_ms(500);

  nrfx_pwm_stop(&PWM_INST, true);
}

void chomp(){

  play_tone(440);
  nrf_delay_ms(750);
  
  play_tone(880);
  nrf_delay_ms(750);

  nrfx_pwm_stop(&PWM_INST, true);
}

void soundcheck(){
   ascend();
   nrf_delay_ms(1000);
   chomp();
   nrf_delay_ms(1000);
   descend();
}
