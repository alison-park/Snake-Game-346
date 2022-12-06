#include "gesture_driver.h"
#include <stddef.h>

#include <math.h>
#include <stdlib.h>

#include "nrf_delay.h"
#include "nrf_twi_mngr.h"
#include "microbit_v2.h"

static const nrf_twi_mngr_t* i2c_manager = NULL;

uint8_t gestCnt, UCount, DCount, LCount, RCount;
enable_cfg _enable;
gconf1 _gconf1;
gconf2 _gconf2;
gconf3 _gconf3;
gconf4 _gconf4;
gpulse _gpulse;
gstatus _gstatus;
status _status;

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
  printf("Gesture sensor connected\n");
  
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

  enableColor(true);
}

uint8_t gest_reg_read(uint8_t i2c_addr, uint8_t reg_addr){
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

uint16_t read16R(uint8_t reg) {
  uint8_t ret[2];
  gest_reg_read_to_buffer(reg, ret, 2);

  return (ret[1] << 8) | ret[0];
}

uint8_t gest_reg_read_to_buffer(uint8_t reg, uint8_t *buf, uint8_t num) {
  buf[0] = reg;
  uint8_t count = 0;
  while(count < num){
    count+= 1;
    
    buf[count] = gest_reg_read(I2C_GEST_ADDR, reg);
  }
  return num;
}

void gest_reg_write(uint8_t i2c_addr, uint8_t reg_addr, uint8_t data){
  uint8_t write_data[] = { reg_addr, data }; 
  nrf_twi_mngr_transfer_t write_transfer[] = {
    NRF_TWI_MNGR_WRITE(i2c_addr, &write_data, 2, 0)
  };
    
  nrf_twi_mngr_perform(i2c_manager, NULL, write_transfer, 1, NULL);
}

void resetCounts() {
  gestCnt = 0;
  UCount = 0;
  DCount = 0;
  LCount = 0;
  RCount = 0;
}

// config returns
uint8_t enable_get() {
  return (_enable.GEN << 6) | (_enable.PIEN << 5) | (_enable.AIEN << 4) | (_enable.WEN << 3) | (_enable.PEN << 2) |(_enable.AEN << 1) | _enable.PON;
};

uint8_t gconf1_get() { return (_gconf1.GFIFOTH << 6) | (_gconf1.GEXMSK << 2) | _gconf1.GEXPERS; }

uint8_t gconf2_get() { return (_gconf2.GGAIN << 5) | (_gconf2.GLDRIVE << 3) | _gconf2.GWTIME; }

uint8_t gpulse_get() { return (_gpulse.GPLEN << 6) | _gpulse.GPULSE; }
    
uint8_t gconf3_get() { return _gconf3.GDIMS; }

uint8_t gconf4_get() { return (_gconf4.GIEN << 1) | _gconf4.GMODE; }

void gstatus_set(uint8_t data) {
  _gstatus.GFOV = (data >> 1) & 0x01;
  _gstatus.GVALID = data & 0x01;
}

void gconf4_set(uint8_t data) {
  _gconf4.GIEN = (data >> 1) & 0x01;
  _gconf4.GMODE = data & 0x01;
}

void status_set(uint8_t data) {
  _status.AVALID = data & 0x01;
  _status.PVALID = (data >> 1) & 0x01;
  _status.GINT = (data >> 2) & 0x01;
  _status.AINT = (data >> 4) & 0x01;
  _status.PINT = (data >> 5) & 0x01;
  _status.PGSAT = (data >> 6) & 0x01;
  _status.CPSAT = (data >> 7) & 0x01;
}

void enable(bool en) {
  _enable.PON = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

void enableGesture(bool en) {
  if (!en) {
    _gconf4.GMODE = 0;
    gest_reg_write(I2C_GEST_ADDR, GCONF4, gconf4_get());
  }
  _enable.GEN = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
  resetCounts();
}

void enableProximity(bool en) {
  _enable.PEN = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

void disableProximityInterrupt() {
  _enable.PIEN = 0;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

void enableColor(bool en) {
  _enable.AEN = en;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

void clearInterrupt() {
  gest_reg_write(I2C_GEST_ADDR, AI_CLEAR, 0);
}

void enableColorInterrupt() {
  _enable.AIEN = 1;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}

void disableColorInterrupt() {
  _enable.AIEN = 0;
  gest_reg_write(I2C_GEST_ADDR, ENABLE_REG, enable_get());
}


bool colorDataReady() {
  status_set(gest_reg_read(I2C_GEST_ADDR, STATUS));
  return _status.AVALID;
}

color getColorData() {
  uint8_t CL = gest_reg_read(I2C_GEST_ADDR, CDATAL);
  uint8_t CH = gest_reg_read(I2C_GEST_ADDR, CDATAH);
  
  uint16_t c = (CH << 8) | CL;
  
  uint8_t RL = gest_reg_read(I2C_GEST_ADDR, RDATAL);
  uint8_t RH = gest_reg_read(I2C_GEST_ADDR, RDATAH);
  
  uint16_t r = (RH << 8) | RL;
  
  uint8_t GL = gest_reg_read(I2C_GEST_ADDR, GDATAL);
  uint8_t GH = gest_reg_read(I2C_GEST_ADDR, GDATAH);
  
  uint16_t g = (GH << 8) | GL;
  
  uint8_t BL = gest_reg_read(I2C_GEST_ADDR, BDATAL);
  uint8_t BH = gest_reg_read(I2C_GEST_ADDR, BDATAH);
  
  uint16_t b = (BH << 8) | BL;
  
  printf("The color is %x, %x, %x\n", r, g, b);
  
  color res_color = {
    .c = c,
    .r = r,
    .g = g,
    .b = b
  };
  
  return res_color;
}



void setGestureDimensions(uint8_t dims) {
  _gconf3.GDIMS = dims;
  gest_reg_write(I2C_GEST_ADDR, GCONF3, gconf3_get());
}

void setGestureFIFOThreshold(uint8_t thresh) {
  _gconf1.GFIFOTH = thresh;
  gest_reg_write(I2C_GEST_ADDR, GCONF1, gconf1_get());
}

void setGestureGain(uint8_t gain) {
  _gconf2.GGAIN = gain;
  gest_reg_write(I2C_GEST_ADDR, GCONF2, gconf2_get());
}

void setGestureProximityThreshold(uint8_t thresh) {
  gest_reg_write(I2C_GEST_ADDR, GPENTH, thresh);
}

bool gestureValid() {
  uint8_t res = gest_reg_read(I2C_GEST_ADDR, GSTATUS);
  gstatus_set(res);
  return _gstatus.GVALID;
}

uint8_t readGesture() {
  uint8_t toRead;
  uint8_t buf[256];
  unsigned long t = 0;
  uint8_t gestureReceived;
  while (1) {
    int up_down_diff = 0;
    int left_right_diff = 0;
    gestureReceived = 0;
    if (!gestureValid()){
      return 0;
    }

    nrf_delay_ms(30);
    toRead = gest_reg_read(I2C_GEST_ADDR, GFLVL);

    // produces sideffects needed for readGesture to work
    gest_reg_read_to_buffer(GFIFO_U, buf, toRead);

    if (abs((int)buf[0] - (int)buf[1]) > 13)
      up_down_diff += (int)buf[0] - (int)buf[1];

    if (abs((int)buf[2] - (int)buf[3]) > 13)
      left_right_diff += (int)buf[2] - (int)buf[3];

    //printf("Up down: %i ", up_down_diff);
    //printf("Right left: %i\n", left_right_diff);
    if (up_down_diff != 0) {
      if (up_down_diff < 0) {
        if (DCount > 0) {
          gestureReceived = UP;
        } else
          UCount++;
      } else if (up_down_diff > 0) {
        if (UCount > 0) {
          gestureReceived = DOWN;
        } else
          DCount++;
      }
    }

    if (left_right_diff != 0) {
      if (left_right_diff < 0) {
        if (RCount > 0) {
          gestureReceived = LEFT;
        } else
          LCount++;
      } else if (left_right_diff > 0) {
        if (LCount > 0) {
          gestureReceived = RIGHT;
        } else
          RCount++;
      }
    }

    if (up_down_diff != 0 || left_right_diff != 0)
      t = 0;


    t+= 100;
    if (gestureReceived || t > 300) {
      resetCounts();
      return gestureReceived;
    }
    //printf("%i\n", t);
  }
}
