#include "gpio.h"

typedef struct{
  uint32_t out;
  uint32_t outset;
  uint32_t outclr;
  uint32_t in;
  uint32_t dir;
  uint32_t dirset;
  uint32_t dirclr;
  uint32_t latch;
  uint32_t detectmode;
  uint32_t _unused_A[0x76];
  uint32_t pin_cnf[32];  

} gpio_reg_t;

volatile gpio_reg_t* PORT_ONE = (gpio_reg_t*)(0x50000504);
volatile gpio_reg_t* PORT_TWO = (gpio_reg_t*)(0x50000804);

void gpio_config(uint8_t gpio_num, gpio_direction_t dir){
  if (gpio_num > 31){
    uint8_t val = 32 - gpio_num;
    *(&(PORT_TWO->pin_cnf)[val])=dir;
  }
  else{
    *(&(PORT_ONE->pin_cnf)[gpio_num])=dir;
  }

}

void gpio_set(uint8_t gpio_num) {
  if(gpio_num > 31) {
    uint8_t val = 32 - gpio_num;
    *(&(PORT_TWO->out)) |= 1 << val;
  }
  else {
    *(&(PORT_ONE->out)) |= 1 << gpio_num;
  }
}

void gpio_clear(uint8_t gpio_num) {
  if(gpio_num > 31){
    uint8_t val = 32 - gpio_num;
    *(&(PORT_TWO->out)) &= 0 << val;
  }
  else {
    *(&(PORT_ONE->out)) &= 0 << gpio_num;
  }
}

bool gpio_read(uint8_t gpio_num) {
  uint32_t pin_val;
  uint8_t val = gpio_num;
  if(gpio_num > 31){
    val = 32 - gpio_num;
    pin_val = *(&(PORT_TWO->in));
  }
  else{
    pin_val = *(&(PORT_ONE->in));
  }

  return (pin_val&(1 << val));
}

