#pragma once

#include <stddef.h>
#include "nrf_twi_mngr.h"

static const uint8_t I2C_ADDR_OLED = 0x3D;
static const uint8_t I2C_COMMAND = 0x00;
static const uint8_t I2C_DATA = 0x40;

static const uint8_t CHARGEPUMP = 0x8D;
static const uint8_t COMSCANINC = 0xC0;
static const uint8_t COMSCANDEC = 0xC8;
static const uint8_t DISPLAYOFF = 0xAE;
static const uint8_t DISPLAYON = 0xAF;
static const uint8_t DISPLAYALLON = 0xA5;
static const uint8_t DISPLAYALLONRESUME = 0xA4;
static const uint8_t EXTERNALVCC = 0x01;
static const uint8_t INVERTDISPLAY = 0xA7;
static const uint8_t MEMORYMODE = 0x20;
static const uint8_t NORMALDISPLAY = 0xA6;
static const uint8_t SETCONTRAST = 0x81;
static const uint8_t SETCOMPINS = 0xDA;
static const uint8_t SETVCOMDESELECT = 0xDB;
static const uint8_t SETDISPLAYCLOCKDIV = 0xD5;
static const uint8_t SETDISPLAYOFFSET = 0xD3;
static const uint8_t SETLOWCOLUMN = 0x00;
static const uint8_t SETHIGHCOLUMN = 0x10;
static const uint8_t SETMULTIPLEX = 0xA8;
static const uint8_t SETPRECHARGE = 0xD9;
static const uint8_t SETSTARTLINE = 0x40;
static const uint8_t SEGREMAP = 0xA0;
static const uint8_t SWITCHCAPVCC = 0x02;

static const uint8_t LCDWIDTH = 64;
static const uint8_t LCDHEIGHT = 48;


void init_qwiic_led_driver(const nrf_twi_mngr_t* i2c);

void i2c_reg_write_oled(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data);

void setPageAddress(uint8_t add);

void setColumnAddress(uint8_t add);

void data(uint8_t c);

void command(uint8_t c);

void display();

void setPixel(uint8_t x, uint8_t y, uint8_t on);

