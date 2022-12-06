#pragma once

#include <stddef.h>
#include <math.h>
#include "nrf_twi_mngr.h"

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


static const uint8_t CDATAL = 0x94;
static const uint8_t CDATAH = 0x95;
static const uint8_t RDATAL = 0x96;
static const uint8_t RDATAH = 0x97;
static const uint8_t GDATAL = 0x98;
static const uint8_t GDATAH = 0x99;
static const uint8_t BDATAL = 0x9A;
static const uint8_t BDATAH = 0x9B;


static const uint8_t GFLVL = 0xAE;
static const uint8_t GFIFO_U = 0xFC;


static const uint8_t LEFT = 0x03;
static const uint8_t RIGHT = 0x04;
static const uint8_t UP = 0x01;
static const uint8_t DOWN = 0x02;

typedef struct{
    uint16_t c;
    uint16_t r;
    uint16_t g;
    uint16_t b;

} color;

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
    uint8_t GEXPERS : 2;

    uint8_t GEXMSK : 4;

    uint8_t GFIFOTH : 2;
} gconf1;

typedef struct{
    uint8_t GWTIME : 3;

    uint8_t GLDRIVE : 2;

    uint8_t GGAIN : 2;
} gconf2;

typedef struct {
    uint8_t GPULSE : 6;

    uint8_t GPLEN : 2;
} gpulse;

typedef struct {
    uint8_t GDIMS : 2;
} gconf3;

typedef struct {
    uint8_t GMODE : 1;

    uint8_t GIEN : 2;
} gconf4;

typedef struct{
    uint8_t GVALID : 1;

    uint8_t GFOV : 1;
} gstatus;

typedef struct{
    uint8_t AVALID : 1;

    uint8_t PVALID : 1;

    uint8_t GINT : 1;

    uint8_t AINT : 1;

    uint8_t PINT : 1;

    uint8_t PGSAT : 1;

    uint8_t CPSAT : 1;
} status;

void init_gesture(const nrf_twi_mngr_t* i2c);

uint8_t gest_reg_read(uint8_t i2c_addr, uint8_t reg_addr);

uint16_t read16R(uint8_t reg);

void gest_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);

uint8_t gest_reg_read_to_buffer(uint8_t reg, uint8_t *buf, uint8_t num);

void resetCounts();
// config returns
uint8_t enable_get();

uint8_t gconf1_get();

uint8_t gconf2_get();

uint8_t gpulse_get();
    
uint8_t gconf3_get();

uint8_t gconf4_get();

void gconf4_set(uint8_t data);

void enable(bool en);

void enableGesture(bool en);

void enableProximity(bool en);

void disableProximityInterrupt();

void enableColor(bool en);

void clearInterrupt();

void enableColorInterrupt();

void disableColorInterrupt();

bool colorDataReady();

color getColorData();

void setGestureDimensions(uint8_t dims);

void setGestureFIFOThreshold(uint8_t thresh);

void setGestureGain(uint8_t gain);

void setGestureProximityThreshold(uint8_t thresh);

uint8_t readGesture();

