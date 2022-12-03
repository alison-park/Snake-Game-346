#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"

#include "microbit_v2.h"
#include "qwiic_led_driver.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

void init_qwiic_led_driver(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;
  printf("%x\n", i2c);
  
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;

  nrf_twi_mngr_init(i2c, &i2c_config);
  
  printf("init");
  
  // to initiate communication pull


  // write to make the display turn off
  command(DISPLAYOFF);
  printf("Turned display off\n");
  
  command(SETDISPLAYCLOCKDIV);
  command(0x80);
  
  command(SETMULTIPLEX);
  command(0x2F);
  
  command(SETDISPLAYOFFSET);
  command(0x0);
  
  command(SETSTARTLINE | 0x0);
  
  command(CHARGEPUMP);
  command(0x14);
  
  command(NORMALDISPLAY);
  command(DISPLAYALLONRESUME);
  
  command(SEGREMAP | 0x1);
  command(COMSCANDEC);
  
  command(SETCOMPINS);
  command(0x12);
  
  command(SETCONTRAST);
  command(0x8F);
  
  command(SETPRECHARGE);
  command(0xF1);
  
  command(SETVCOMDESELECT);
  command(0x40);
  
  command(DISPLAYON);
  
  // clear
  clear();

 }
 
void data(uint8_t c){
  i2c_reg_write_oled(I2C_ADDR_OLED, I2C_DATA, c);
}

void command(uint8_t c){
  i2c_reg_write_oled(I2C_ADDR_OLED, I2C_COMMAND, c);
}

void i2c_reg_write_oled(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data){
  uint8_t write_data[] = { reg_addr, data }; 
  nrf_twi_mngr_transfer_t write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &write_data, 2, 0)
  };
    
  uint8_t error = nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
  
  printf("Error %d\n", error);
}   

void clear(){
  for (int i=0;i<8; i++) {
    setPageAddress(i);
    setColumnAddress(0);
    for (int j=0; j<0x80; j++) {
	data(0);
    }
  }
}

void setPageAddress(uint8_t add){
  command(0xb0 | add);
}

void setColumnAddress(uint8_t add){
  command(((0x10|(add>>4))+0x02));
  command((0x0f&add));
}




