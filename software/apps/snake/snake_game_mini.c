#include "snake_game_mini.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <linked_list.h>
#include "play_sound.h"
#include "imu_driver.h"
#include "qwiic_led_driver.h"
#include "gesture_driver.h"

#include "microbit_v2.h"
#include "nrf_twi_mngr.h"
#include "app_timer.h"
#include "nrf_delay.h"

// app timers for the sound
APP_TIMER_DEF(start_sound);

game_timer_id;

bool gameover = false;

int i,j,height = 48, width = 64, score;
int fruit[4][2] = {0};
int dir[2] = {-1, 0};
int grid[64][48] = {0};

uint32_t seed = 0;

void update_grid(){
  logic();
  // clear board + update it based on linked list
  for(i=0; i< width; i++){
    for(j=0; j< height; j++){
      grid[i][j] = 0;
    }
  }
  node_t* first=list_get_first();
  while(first!=NULL){
  
    //printf("adding (%i, %i) as a snake \n", first->x, first->y);
    grid[first->x][first->y]=1;
    first=first->next;
  }
  
  for(i = 0; i < 4; i++){
    grid[fruit[i][0]][fruit[i][1]] = 2;
  }
  
}

void draw(){
  update_grid();
  
  color c = getColorData();
  
  if(c.r == 0 && c.g == 0 && c.b == 0){
    invertColors();
  }
  
  for (i=0; i<width; i++){ //column, toggle to turn on column
    for (j=0; j<height; j++){ //row, toggle to turn on specific LED
      //set led to value of grid[width][height]
      if (grid[i][j] == 1 | grid[i][j] == 2){
      	setPixel(i, j, 1);
      	//printf("Setting(%i, %i)\n", i, j);
      }
      else{
      	setPixel(i, j, 0);
      }
    }
  }
  
  display();
}

void setup(app_timer_id_t timer_id){
  game_timer_id = timer_id;
  
  // seed the generator
  srand(seed);
  reset_list();
  
  dir[0] = -1;
  dir[1] = 0;
  
  gameover = false;

  node_t* threeNode = malloc(sizeof(node_t));
  threeNode->x = 56;
  threeNode->y = 34;
  list_insert(threeNode);
  node_t* twoNode = malloc(sizeof(node_t));
  twoNode->x = 55;
  twoNode->y = 34;
  list_insert(twoNode);
  
  node_t* oneNode = malloc(sizeof(node_t));
  oneNode->x = 54;
  oneNode->y = 34;
  list_insert(oneNode);

  // generate fruit positions
  generateFruit();
  
  // draw start screen
  resetColorMode();
  drawStart();
  
  // slight start pause
  nrf_delay_ms(300);
  
  // play the ascend sound
  ascend();
}

void gameOver() {
  seed = app_timer_cnt_get();
  app_timer_stop(game_timer_id);
  gameover= true;
  drawEnd();
  descend();
}

int checkCollisions() {
  node_t* head = list_get_first();
  node_t* curr = list_get_first();
  curr = curr -> next;

  while (curr != NULL) {
    if (curr->x == head->x && curr->y == head->y){
      return 1;
    }

    curr = curr->next;
  }

  return 0;
}

// WE NEED TO DO THIS STILL
void generateFruit() {
  // set new coordinates for fruit
  // re generate them until they don't interect with snake
  bool collide = true;
  int left;
  int top;
  
  while (collide) {
    left = rand() % 63;
    top = rand() % 47;
    
    printf("Generating (%i, %i) \n", left, top);
    
    node_t* curr = list_get_first();
    collide = false;
    while( curr != NULL){
      if((curr->x == left || curr->x == (left+1)) && (curr->y == top || curr->y == (top+1))){
        collide = true;
        break;
      } 
      curr = curr -> next;
    }
  }
  
  //printf("I am done");
  
  fruit[0][0] = left;
  fruit[0][1] = top;
  
  fruit[1][0] = left;
  fruit[1][1] = top + 1;
  
  fruit[2][0] = left + 1;
  fruit[2][1] = top;
  
  fruit[3][0] = left + 1;
  fruit[3][1] = top + 1;
  
}

bool eatFruit(){
  node_t* first=list_get_first();
  
  //check each part of fruit
  if ((first->x == fruit[0][0] || first->x == fruit[2][0]) && (first->y == fruit[0][1] || first->y == fruit[1][1])){
    chomp();
    return true;
  }
  
  return false;
}

void logic(){
  // check if direction changes with sensor
  uint8_t direction = read_tilt();
  //printf("New direction is %i\n", direction);
  if (direction == 1) {
    dir[0] = 0;
    dir[1] = 1;
  }
  else if (direction == 2) {
    dir[0] = 0;
    dir[1] = -1;
  }
  else if (direction == 3){
    dir[0] = 1;
    dir[1] = 0;
  }
  else if (direction == 4) {
    dir[0] = -1;
    dir[1] = 0;
  }
  
  // update head of snake
  node_t* first=list_get_first();
  node_t* newNode = malloc(sizeof(node_t));
  if ((first->x + dir[0]) >= 0 && (first->x + dir[0]) < width){
    newNode->x = first->x + dir[0];
  }
  else {
    // we hit a wall
    printf("We hit a wall");
    gameOver();
    return;
  }
  
  if ((first->y + dir[1]) >= 0 && (first->y + dir[1]) < height){
    newNode->y = first->y + dir[1];
  }
  else {
    // we hit a wall
    printf("We hit a wall");
    gameOver();
    return;
  }

  list_insert(newNode);

  // check if we eat a fruit
  if (eatFruit()){
    score += 1;
    // generate a new fruit
    generateFruit();
    
    // don't pop the tail and also add one in direction we're going
    node_t* first=list_get_first();
    node_t* newNode = malloc(sizeof(node_t));
    
    newNode->x = (first->x + dir[0]);
    newNode->y = (first->y + dir[1]);
    
    list_insert(newNode);
  }
  else {
    // pop tail of snake
    node_t* last = list_remove_last();
    //printf("Removing (%i, %i)\n", last->x, last->y);
    free(last);
  }
  
  // check for collisions
  if(checkCollisions()) {
    // we hit ourselves
    printf("We hit ourselves");
    gameOver();
    return;
  }
  
  
}
