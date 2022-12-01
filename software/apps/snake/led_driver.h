#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "nrf.h"

static void pwm_init(void);

static void display_array(int arr[32][8]);

static void timer_init(void);
