#include "imu_driver.h"
#include <stddef.h>
#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

void init_imu(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;
  //printf("%x\n", i2c);
  
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;

  nrf_twi_mngr_init(i2c, &i2c_config);

// Configure accelerometer at 104Hz, normal mode
  i2c_reg_write(I2C_ADDR, CTRL1_XL, 0x40);

  // Read WHO AM I register
  uint8_t result = i2c_reg_read(I2C_ADDR, WHO_AM_I_ADDR);
  //should be Ox6C
  printf("Please be correct : %x\n", result);
  
 }

uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr){
  uint8_t rx_buf = 0;
  //printf("%x\n", i2c_manager);
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &reg_addr, 1, NRF_TWI_MNGR_NO_STOP),
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 1, 0)
  };
  uint32_t res = nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 2, NULL);

  //printf("Error code %d\n", res);

  return rx_buf;
}   

void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data){
  uint8_t write_data[] = { reg_addr, data }; 
  nrf_twi_mngr_transfer_t write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &write_data, 2, 0)
  };
    
  nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}   

imu_measurement_t read_tilt(){
  imu_measurement_t result = {0};
  //The value is expressed as a 16-bit word in two’s complement.
  // x axis measurements
  uint8_t x1 = i2c_reg_read(I2C_ADDR, OUTX_L_A);
  uint8_t x2 = i2c_reg_read(I2C_ADDR, OUTX_H_A);
  int16_t final_x = (uint16_t)(x2 << 8) + x1;

  uint8_t y1 = i2c_reg_read(I2C_ADDR, OUTY_L_A);
  uint8_t y2 = i2c_reg_read(I2C_ADDR, OUTY_H_A);
  int16_t final_y = (uint16_t)(y2 << 8) + y1;

  uint8_t z1 = i2c_reg_read(I2C_ADDR, OUTZ_L_A);
  uint8_t z2 = i2c_reg_read(I2C_ADDR, OUTZ_H_A);
  int16_t final_z = (uint16_t)(z2 << 8) + z1;


  
  // then return the results
  result.x = (final_x * 0.061)/1000;
  result.y = (final_y * 0.061)/1000;
  result.z = (final_z * 0.061)/1000;
  //
  return result;
}
