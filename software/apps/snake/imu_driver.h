#pragma once

#include <stddef.h>
#include "nrf_twi_mngr.h"

typedef struct {
  float x;
  float y;
  float z;
} imu_measurement_t;

void init_imu(const nrf_twi_mngr_t* i2c);

uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr);

void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr);
