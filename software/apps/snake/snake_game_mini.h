#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_timer.h"

void setup(app_timer_id_t timer_id);
void draw();
void update_grid();
void logic();
void generateFruit();
bool eatFruit();
int checkCollisions();
void gameOver();
