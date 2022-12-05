#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_timer.h"

void update_grid();
void draw();
void setup(app_timer_id_t timer_id);
int checkCollisions();
void logic();
void generateFruit();
