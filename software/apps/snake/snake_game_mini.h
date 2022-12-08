#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"
#include "app_timer.h"

// SETUP THE GAME
void setup(app_timer_id_t timer_id);

// UPDATE GAME STATES EACH FRAME
void draw();
void update_grid();
void logic();

// LOGIC HELPERS
void generateFruit();
bool eatFruit();
int checkCollisions();

// GAME OVER
void gameOver(bool good);
