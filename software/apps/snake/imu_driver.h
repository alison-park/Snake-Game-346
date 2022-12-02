#pragma once

#include <stddef.h>
#include "nrf_twi_mngr.h"

static const uint8_t I2C_ADDR = 0x6B;
static const uint8_t WHO_AM_I_ADDR = 0x0F;
static const uint8_t CTRL1_XL = 0x10;
static const uint8_t OUTX_L_A = 0x28;
static const uint8_t OUTY_L_A = 0x2A;
static const uint8_t OUTZ_L_A = 0x2C;
static const uint8_t OUTX_H_A = 0x29;
static const uint8_t OUTY_H_A = 0x2B;
static const uint8_t OUTZ_H_A = 0x2D;

typedef struct {
  float x;
  float y;
  float z;
} imu_measurement_t;

void init_imu(const nrf_twi_mngr_t* i2c);

uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr);

void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr. uint8_t data);

imu_measurement_t read_tilt();