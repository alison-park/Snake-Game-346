#include "imu_driver.h"
#include <stddef.h>
#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

void init_imu(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;
  printf("%x\n", i2c);
  
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;
  nrf_twi_mngr_init(i2c, &i2c_config);

  // Read WHO AM I register
  //should be OxEA
  uint8_t result = i2c_reg_read(0x68, 0x00);
  printf("Please be correct : %x\n", result);
  
 }

uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr){
  uint8_t rx_buf = 0;
  printf("%x\n", i2c_manager);
  nrf_twi_mngr_transfer_t const read_transfer[] = {
    NRF_TWI_MNGR_READ(i2c_addr, &rx_buf, 2, 0)
  };
  nrf_twi_mngr_perform(i2c_manager, NULL, read_transfer, 1, NULL);

  return rx_buf;
}   

void i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr){
  uint8_t write_data[2] = { reg_addr, 0 }; 
  nrf_twi_mngr_transfer_t write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &write_data, 2, NRF_TWI_MNGR_NO_STOP)
  };
    
  nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}   

imu_measurement_t read_tilt(uint8_t i2c_addr){
  imu_measurement_t result = {0};
  
  // x axis measurements
  int16_t x_axis_l = i2c_reg_read(i2c_addr, 46);
  int16_t x_axis_h = i2c_reg_read(i2c_addr, 45);

  x_axis_h = x_axis_h <<8;
  int16_t concat_x = x_axis_h + x_axis_l;
  float result_x = (float) ( concat_x >> 6 );
  
  // y axis measurements
  int16_t x_axis_l = i2c_reg_read(i2c_addr, 46);
  int16_t x_axis_h = i2c_reg_read(i2c_addr, 45);

  x_axis_h = x_axis_h <<8;
  int16_t concat_x = x_axis_h + x_axis_l;
  float result_x = (float) ( concat_x >> 6 );
  
  // z axis measurements
  int16_t x_axis_l = i2c_reg_read(i2c_addr, 46);
  int16_t x_axis_h = i2c_reg_read(i2c_addr, 45);

  x_axis_h = x_axis_h <<8;
  int16_t concat_x = x_axis_h + x_axis_l;
  float result_x = (float) ( concat_x >> 6 );
  
  

}
