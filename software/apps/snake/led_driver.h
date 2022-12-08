#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"

static const int buffer_size = 696;
static const int num_leds = 24;

void pwm_init(void);

void display_array(int* arr);
