#include "imu_driver.h"
#include <stddef.h>
#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

void init_imu(){
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priortiy = 0;
  nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
  /*setAccelRange(8);
    setAccelDataRate(416);
    setGyroRange(500);
    setGyroDataRate(416);
    setBlockDataUpdate(true);*/
}

static uint8_t i2c_reg_read(uint8_t i2c_addr, uint8_t reg_addr){


}   

static uint8_t i2c_reg_write(uint8_t i2c_addr, uint8_t reg_addr){


}   
