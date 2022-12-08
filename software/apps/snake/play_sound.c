
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
  
     nrfx_pwm_stop(&PWM_INST, true);

     NRF_PWM0->COUNTERTOP = 500000/frequency;
     
     sequence_data[0] =(500000/frequency)/2;
     
     nrfx_pwm_simple_playback(&PWM_INST, &pwm_sequence, 10, NRFX_PWM_FLAG_LOOP);
}   


void ascend(){
  nrfx_pwm_stop(&PWM_INST, true);
  
  play_tone(440);
  nrf_delay_ms(250);

  play_tone(554);
  nrf_delay_ms(250);

  play_tone(659);
  nrf_delay_ms(250);

  play_tone(880);
  nrf_delay_ms(250);

  nrfx_pwm_stop(&PWM_INST, true);
}


void descend(){
  nrfx_pwm_stop(&PWM_INST, true);

  play_tone(880);
  nrf_delay_ms(250);
  
  play_tone(659);
  nrf_delay_ms(250);

  play_tone(554);
  nrf_delay_ms(250);

  play_tone(440);
  nrf_delay_ms(250);

  nrfx_pwm_stop(&PWM_INST, true);
}

void chomp(){
  nrfx_pwm_stop(&PWM_INST, true);

  play_tone(1760);
  nrf_delay_ms(100);
  
  play_tone(1567);
  nrf_delay_ms(100);

  play_tone(1396);
  nrf_delay_ms(100);

  nrfx_pwm_stop(&PWM_INST, true);
}

void soundcheck(){
   ascend();
   nrf_delay_ms(1000);
   chomp();
   nrf_delay_ms(1000);
   descend();
}
