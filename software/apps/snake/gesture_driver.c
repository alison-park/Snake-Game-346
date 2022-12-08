#include "gesture_driver.h"
#include <stddef.h>

#include <math.h>
#include <stdlib.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

// VARIABLES FOR KEEPING TRACK OF STATUS
uint8_t gestCnt, UCount, DCount, LCount, RCount;
enable_cfg _enable;
gconf4 _gconf4;

// INITIALIZATION FUNCTION -------------------------------------------------------------------
void init_gesture(const nrf_twi_mngr_t* i2c){
  i2c_manager = i2c;
  
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;

  nrf_twi_mngr_init(i2c, &i2c_config);

  // Read WHO AM I register
  uint8_t result = gest_reg_read(I2C_GEST_ADDR, GEST_WHO_AM_I);
  //should be OxAB
  if(result != 0xAB){
    printf("Failure to connect!\n");
    return;
  }
  
  // disable everything to start
  enableGesture(false);
  enableProximity(false);
  enableColor(false);

  disableColorInterrupt();
  disableProximityInterrupt();
  clearInterrupt();

  // By default, the device is in power down mode on bootup 
  enable(false);
  nrf_delay_ms(100);
  enable(true);
  nrf_delay_ms(100);

  // enable color sensor (which is what we use)
  enableColor(true);
}

// BASIC READ AND WRITE FUNCTION -------------------------------------------------------------------
uint8_t gest_reg_read(uint8_t i2c_addr, uint8_t reg_addr){
  uint8_t rx_buf = 0;
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
  };
  nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);
  
  return rx_buf;
}

void gest_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data){
  uint8_t write_data[] = { reg_addr, data }; 
  nrf_twi_mngr_transfer_t write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &write_data, 2, 0)
  };
    
  nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}

// CONTROLS FOR THE STATUS STRUCTS -------------------------------------------------------------------
void resetCounts() {
  gestCnt = 0;
  UCount = 0;
  DCount = 0;
  LCount = 0;
  RCount = 0;
}

uint8_t enable_get() {
  return (_enable.GEN << 6) | (_enable.PIEN << 5) | (_enable.AIEN << 4) | (_enable.WEN << 3) | (_enable.PEN << 2) |(_enable.AEN << 1) | _enable.PON;
};

uint8_t gconf4_get() { return (_gconf4.GIEN << 1) | _gconf4.GMODE; }

// ENABLING/DISABLING THE VARIOUS ASPECTS OF THE SENSORS -------------------------------------------------------------------
// enable device
void enable(bool en) {
  _enable.PON = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

// enable (or disable) gesture sensor
void enableGesture(bool en) {
  if (!en) {
    _gconf4.GMODE = 0;
    gest_reg_write(I2C_GEST_ADDR, GCONF4, gconf4_get());
  }
  _enable.GEN = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
  resetCounts();
}

// enable (or disable) proximity sensor
void enableProximity(bool en) {
  _enable.PEN = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

// enable (or disable) color sensor
void enableColor(bool en) {
  _enable.AEN = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

// clear interrupts
void clearInterrupt() {
  gest_reg_write(I2C_GEST_ADDR, AI_CLEAR, 0);
}

// disable the proximity interrupt
void disableProximityInterrupt() {
  _enable.PIEN = 0;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

// disable the color interrupt
void disableColorInterrupt() {
  _enable.AIEN = 0;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

// READING COLOR DATA FROM THE SENSOR -------------------------------------------------------------------
// read color data from registers and process into useable return
color getColorData() {
  uint8_t CL = gest_reg_read(I2C_GEST_ADDR, CDATAL);
  uint8_t CH = gest_reg_read(I2C_GEST_ADDR, CDATAH);
  
  uint16_t c = (CH << 8) | CL;
  
  // red
  uint8_t RL = gest_reg_read(I2C_GEST_ADDR, RDATAL);
  uint8_t RH = gest_reg_read(I2C_GEST_ADDR, RDATAH);
  
  uint16_t r = (RH << 8) | RL;
  
  // green
  uint8_t GL = gest_reg_read(I2C_GEST_ADDR, GDATAL);
  uint8_t GH = gest_reg_read(I2C_GEST_ADDR, GDATAH);
  
  uint16_t g = (GH << 8) | GL;
  
  //blue
  uint8_t BL = gest_reg_read(I2C_GEST_ADDR, BDATAL);
  uint8_t BH = gest_reg_read(I2C_GEST_ADDR, BDATAH);
  
  uint16_t b = (BH << 8) | BL;
  
  // format data for return
  color res_color = {
    .c = c,
    .r = r,
    .g = g,
    .b = b
  };
  
  return res_color;
}