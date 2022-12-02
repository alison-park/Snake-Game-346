#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"

void pwm_init(void);

void display_array(int* arr);

void timer_init(void);
