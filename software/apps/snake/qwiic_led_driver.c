#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"

#include "microbit_v2.h"
#include "qwiic_led_driver.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

// are we in inverse mode or not?
uint8_t mode = 1;

// are we locked from triggering inverse mode?
uint8_t lock = 0;

// INITIALIZE THE OLED -------------------------------------------------------------------
void init_qwiic_led_driver(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;
  
  nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
  i2c_config.scl = EDGE_P19;
  i2c_config.sda = EDGE_P20;
  i2c_config.frequency = NRF_TWIM_FREQ_100K;
  i2c_config.interrupt_priority = 0;

  nrf_twi_mngr_init(i2c, &i2c_config);

  // write all of these commands to set up the display settings
  command(DISPLAYOFF);
  
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
  
  // clear the pixels on the screen
  clear();
}
 
// FUNCTIONS FOR WRITING DATA TO THE OLED ------------------------------------------------------
// write data to the oled
void data(uint8_t c){
  i2c_reg_write_oled(I2C_ADDR_OLED, I2C_DATA, c);
}

// write a command to the oled
void command(uint8_t c){
  i2c_reg_write_oled(I2C_ADDR_OLED, I2C_COMMAND, c);
}

// basic write command
void i2c_reg_write_oled(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data){
  uint8_t write_data[] = { reg_addr, data }; 
  nrf_twi_mngr_transfer_t write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &write_data, 2, 0)
  };
    
  nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}   

void setPageAddress(uint8_t add){
  command(0xb0 | add);
}

void setColumnAddress(uint8_t add){
  command(((0x10|(add>>4))+0x02));
  command((0x0f&add));
}

// VARIABLE TO RECORD THE STATE OF PIXELS ON THE SCREEN ----------------------------------------------------
static uint8_t screenmemory [] = {
// ROW0, BYTE0 to BYTE63
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF8, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0x0F, 0x07, 0x07, 0x06, 0x06, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

// ROW1, BYTE64 to BYTE127
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x81, 0x07, 0x0F, 0x3F, 0x3F, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFE, 0xFC, 0xFC, 0xFC, 0xFE, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xE0,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

// ROW2, BYTE128 to BYTE191
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xFD, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

// ROW3, BYTE192 to BYTE255
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x07, 0x01,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

// ROW4, BYTE256 to BYTE319
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x0F,
0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

// ROW5, BYTE320 to BYTE383
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
0x7F, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// CONTROLLING THE DISPLAY
// set all pixels on the screen to off and display
void clear(){
  for (int i=0;i<8; i++) {
    setPageAddress(i);
    setColumnAddress(0);
    for (int j=0; j<0x80; j++) {
	data(0);
    }
  }

  memset(screenmemory,0,384);			// (64 x 48) / 8 = 384
  display();
}

// display whatever is in the pixel buffer by writing to the oled
void display(){
  for (int i=0; i<6; i++) {
    setPageAddress(i);
    setColumnAddress(0);
    for (int j=0;j<0x40;j++) {
      data(screenmemory[i*0x40+j]);
    }
  }
}

// reset color mode to normal
void resetColorMode(){
  mode = 1;
}

// lock and unlock invert mode as an option
void toggleInvertLock(){
  if (lock == 1){ lock = 0; }
  else { lock = 1; }
}

// switch the color mode to invert or not (if unlocked)
void invertColors(){
 if(lock == 0){
  if (mode == 1){ mode = 0; }
  else { mode = 1; }
  }
}

// set pixel (x,y) to be on/off in the buffer
// this doesn't actually write to the oled!
void setPixel(uint8_t x, uint8_t y, uint8_t on){
  if(mode){
    if(on == 1){
      screenmemory[x + (y/8) * LCDWIDTH] |= (1 << (y % 8));
    }
    else {
      screenmemory[x+ (y/8)*LCDWIDTH] &= ~(1 << (y % 8));
    }
  }
  else {
    if(on == 0){
      screenmemory[x + (y/8) * LCDWIDTH] |= (1 << (y % 8));
    }
    else {
      screenmemory[x+ (y/8)*LCDWIDTH] &= ~(1 << (y % 8));
    }
  }
}

// TEXT SCREEN MESSAGES ---------------------------------------------------
// use message bitmaps to draw the start message
// "Get ready..."
void drawStart(){
  // draw get
  uint8_t get_message[] = {
  	0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,
  	1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0,
  };
  
  int end_height = 9;
  int end_width = 17;
  
  int startx = 62;
  int starty = 30;
  
  for(int i = 0; i< end_height; i++){
    for(int j = 0; j< end_width; j++){
    	uint8_t on = get_message[i*end_width + j];
    	setPixel(startx - j, starty - i, on); 
    }
  }
  
  // draw ready
  uint8_t ready_message[] = {
  	1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  	1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0
  };
  
  startx -= end_width + 6;
  
  end_height = 9;
  end_width = 38;
  
  for(int i = 0; i< end_height; i++){
    for(int j = 0; j< end_width; j++){
    	uint8_t on = ready_message[i*end_width + j];
    	setPixel(startx - j, starty - i, on); 
    }
  }
  
  display();
}

// use message bitmaps to draw the losing message
// "LOL Loser"
void drawEnd(){
  // draw lol
  uint8_t lol_message[] = {
  	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0,
  	1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0,
  	1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0,
  	1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1
  };
  
  int end_height = 9;
  int end_width = 17;
  
  int startx = 58;
  int starty = 30;
  
  for(int i = 0; i< end_height; i++){
    for(int j = 0; j< end_width; j++){
    	uint8_t on = lol_message[i*end_width + j];
    	setPixel(startx - j, starty - i, on); 
    }
  }
  
  // draw loser
  uint8_t loser_message[] = {
  	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0,
  	1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0,
  	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0,
  	1, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,
  	1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1
  };
  
  startx -= end_width + 6;
  
  end_height = 9;
  end_width = 28;
  
  for(int i = 0; i< end_height; i++){
    for(int j = 0; j< end_width; j++){
    	uint8_t on = loser_message[i*end_width + j];
    	setPixel(startx - j, starty - i, on); 
    }
  }
  
  display();
}

// use message bitmaps to draw the winning message
// "Yay"
void drawGoodEnd(){
  uint8_t yay_message[] = {
  	1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
  	1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1,
  	1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1,
  	0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0,
  	0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0,
  	0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
  	0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
  	0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0,
  	0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0
  };
  
  int end_height = 9;
  int end_width = 17;
  
  int startx = 40;
  int starty = 30;
  
  for(int i = 0; i< end_height; i++){
    for(int j = 0; j< end_width; j++){
    	uint8_t on = yay_message[i*end_width + j];
    	setPixel(startx - j, starty - i, on); 
    }
  }
  
  display();
}

