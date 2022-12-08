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

app_timer_id_t game_timer_id;

int i,j,height = 48, width = 64, score;
int fruit[4][2] = {0};
int dir[2] = {-1, 0};
int grid[64][48] = {0};
bool gameover = false;

uint32_t seed = 1;

// setup the starting game states
void setup(app_timer_id_t timer_id){
  // reset game over
  gameover = false;

  // record the timer id for later stopping
  game_timer_id = timer_id;
  
  // seed the generator
  srand(seed);
  
  // reset the linkedlist
  reset_list();
  
  // set default direction to right
  dir[0] = -1;
  dir[1] = 0;

  // create the beginning snake (3 long)
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
  clear();
  drawStart();
  
  // slight start pause
  nrf_delay_ms(300);
  
  // play the start sound
  ascend();
}

// draw the board on the oled
void draw(){
  // update the board values
  update_grid();
  
  if (gameover){ return; }
  
  // check gesture sensor to see if darkness detected
  // if so invert the pixel display
  color c = getColorData();
  if(c.r == 0 && c.g == 0 && c.b == 0){
    invertColors();
  }
  
  // loop over board to set pixels
  for (i=0; i<width; i++){
    for (j=0; j<height; j++){ 
      if ((grid[i][j] == 1) || (grid[i][j] == 2)){
        // set fruits and snakes to ON
      	setPixel(i, j, 1);
      }
      else{
        // set blanks to OFF
      	setPixel(i, j, 0);
      }
    }
  }
  
  // display the leds
  display();
}

// update grid with fruit and snake coordinates
void update_grid(){
  // update positions, check collisions
  logic();
  
  // clear board
  for(i=0; i< width; i++){
    for(j=0; j< height; j++){
      grid[i][j] = 0;
    }
  }
  
  // add the snake to the board
  node_t* first=list_get_first();
  while(first!=NULL){
  
    grid[first->x][first->y]=1;
    first=first->next;
  }
  
  // add the fruit to the board
  for(i = 0; i < 4; i++){
    grid[fruit[i][0]][fruit[i][1]] = 2;
  }
}

// update all of the game logic
void logic(){
  // check if direction changes with sensor
  uint8_t direction = read_tilt();
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
  
  // check head x position is still in bounds
  if ((first->x + dir[0]) >= 0 && (first->x + dir[0]) < width){
    newNode->x = first->x + dir[0];
  }
  else {
    // we hit a wall
    gameOver(false);
    return;
  }
  
  // check head y position is still in bounds
  if ((first->y + dir[1]) >= 0 && (first->y + dir[1]) < height){
    newNode->y = first->y + dir[1];
  }
  else {
    // we hit a wall
    gameOver(false);
    return;
  }

  list_insert(newNode);

  // check if we eat a fruit
  if (eatFruit()){
    // update score
    score += 1;
    
    // generate a new fruit
    generateFruit();
    
    // don't pop the tail and also add one in direction we're going
    // (grows by two pixels)
    first=list_get_first();
    node_t* extraNode = malloc(sizeof(node_t));
    
    extraNode->x = (first->x + dir[0]);
    extraNode->y = (first->y + dir[1]);
    
    list_insert(extraNode);
  }
  else {
    // pop tail of snake
    node_t* last = list_remove_last();
    free(last);
  }
  
  // check for collisions (see if we hit ourselves)
  if(checkCollisions()) {
    gameOver(false);
    return;
  }
}

// generate a new fruit on the board (not overlapping with the snake)
void generateFruit() {
  bool collide = true;
  int left;
  int top;
  
  // make sure we don't infinitely generate if no space
  int tries = 0;
  
  while (collide) {
    // generate top left of fruit
    // each fruit is 4 pixels
    left = rand() % 63;
    top = rand() % 47;
    
    // make sure snake doesn't collide with any part of fruit
    node_t* curr = list_get_first();
    collide = false;
    while( curr != NULL){
      if((curr->x == left || curr->x == (left+1)) && (curr->y == top || curr->y == (top+1))){
        collide = true;
        break;
      } 
      curr = curr -> next;
    }
    
    // if this happens we can't generate fruit anymore
    // game is over
    tries += 1;
    if (collide && tries >= 100){
      // if you beat our game you still lose
      gameOver(true);
    }
  }
  
  // record the generated fruit position
  fruit[0][0] = left;
  fruit[0][1] = top;
  
  fruit[1][0] = left;
  fruit[1][1] = top + 1;
  
  fruit[2][0] = left + 1;
  fruit[2][1] = top;
  
  fruit[3][0] = left + 1;
  fruit[3][1] = top + 1;
}

// check if the snake eats a fruit
bool eatFruit(){
  node_t* first=list_get_first();
  
  //check each part of fruit to see if overlaps with snake
  if ((first->x == fruit[0][0] || first->x == fruit[2][0]) && (first->y == fruit[0][1] || first->y == fruit[1][1])){
    // we eat the fruit! play the chomp sound
    chomp();
    return true;
  }
  
  return false;
}

// check if the snake collides with itself
int checkCollisions() {
  node_t* head = list_get_first();
  node_t* curr = list_get_first();
  curr = curr -> next;

  // check each snake link to make sure head doesn't collide with it
  while (curr != NULL) {
    if (curr->x == head->x && curr->y == head->y){
      return 1;
    }
    curr = curr->next;
  }

  return 0;
}

// update game states to be game over
void gameOver(bool good) {
  gameover = true;
  // replace the seed (so fruit generation randomizes)
  seed = app_timer_cnt_get();
  
  // stop the timer (so game stops updating)
  app_timer_stop(game_timer_id);
  
  // draw the end screen
  if (good){
    drawGoodEnd();
  }
  else {
    drawEnd();
  }
  
  // play the end sound
  descend();
}
