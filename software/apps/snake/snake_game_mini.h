#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "nrf.h"

void update_grid();
void draw();
void setup();
int checkCollisions();
void logic();
