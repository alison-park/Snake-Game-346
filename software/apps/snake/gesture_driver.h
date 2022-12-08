#pragma once

#include <stddef.h>
#include <math.h>
#include "nrf_twi_mngr.h"

// REGISTERS FOR CONFIGURATION
static const uint8_t I2C_GEST_ADDR = 0x39;
static const uint8_t GEST_WHO_AM_I = 0x92;
static const uint8_t GCONF1 = 0xA2;
static const uint8_t GCONF2 = 0xA3;
static const uint8_t GCONF3 = 0xAA;
static const uint8_t GCONF4 = 0xAB;
static const uint8_t GSTATUS = 0xAF;
static const uint8_t GPENTH = 0xA0;
static const uint8_t GPULSE = 0xA6;
static const uint8_t ENABLE_REG = 0x80;
static const uint8_t AI_CLEAR = 0xE7;
static const uint8_t STATUS = 0x93;

// REGISTERS FOR READING COLOR DATA
static const uint8_t CDATAL = 0x94;
static const uint8_t CDATAH = 0x95;
static const uint8_t RDATAL = 0x96;
static const uint8_t RDATAH = 0x97;
static const uint8_t GDATAL = 0x98;
static const uint8_t GDATAH = 0x99;
static const uint8_t BDATAL = 0x9A;
static const uint8_t BDATAH = 0x9B;

// STRUCT FOR THE RETURN FROM THE READ COLOR FUNCTION
typedef struct{
    uint16_t c;
    uint16_t r;
    uint16_t g;
    uint16_t b;
} color;

// STRUCTS FOR KEEPING TRACK OF CONFIG INFO
typedef struct{
    uint8_t PON : 1;
    uint8_t AEN : 1;
    uint8_t PEN : 1;
    uint8_t WEN : 1;
    uint8_t AIEN : 1;
    uint8_t PIEN : 1;
    uint8_t GEN : 1;
} enable_cfg;

typedef struct {
    uint8_t GMODE : 1;
    uint8_t GIEN : 2;
} gconf4;

// INITIALIZATION FUNCTION
void init_gesture(const nrf_twi_mngr_t* i2c);

// BASIC READ AND WRITE FUNCTION
uint8_t gest_reg_read(uint8_t i2c_addr, uint8_t reg_addr);
void gest_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);

// CONTROLS FOR THE STATUS STRUCTS
void resetCounts();
uint8_t enable_get();
uint8_t gconf4_get();

// ENABLING/DISABLING THE VARIOUS ASPECTS OF THE SENSORS
void enable(bool en);
void enableGesture(bool en);
void enableProximity(bool en);
void enableColor(bool en);
void clearInterrupt();
void disableProximityInterrupt();
void disableColorInterrupt();

// READING COLOR DATA FROM THE SENSOR
color getColorData();
