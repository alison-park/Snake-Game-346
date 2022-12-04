#include "snake_game_mini.h"

#include <stdlib.h>
//#include <conio.h>
#include <stdio.h>
#include <linked_list.h>

#include "imu_driver.h"
#include "qwiic_led_driver.h"


#include "microbit_v2.h"
#include "nrf_twi_mngr.h"


int i,j,height = 48, width = 64, score;
int fruit[4][2] = {0};
int dir[2] = {0, 0};
int grid[64][48] = {0};

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
  for (i=0; i<width; i++){ //column, toggle to turn on column
    for (j=0; j<height; j++){ //row, toggle to turn on specific LED
      //set led to value of grid[width][height]
      if (grid[i][j] == 1 | grid[i][j] == 2){
      	setPixel(i, j, 1);
      	printf("Setting(%i, %i)\n", i, j);
      }
      else{
      	setPixel(i, j, 0);
      }
    }
  }
  
  display();
}

void setup(){
  node_t* threeNode = malloc(sizeof(node_t));
  threeNode->x = 63;
  threeNode->y = 47;
  list_insert(threeNode);
  
  node_t* twoNode = malloc(sizeof(node_t));
  twoNode->x = 62;
  twoNode->y = 47;
  list_insert(twoNode);
  
  node_t* oneNode = malloc(sizeof(node_t));
  oneNode->x = 61;
  oneNode->y = 47;
  list_insert(oneNode);

  
  // generate fruit positions
  fruit[0][0] = 30;
  fruit[0][1] = 30;
  fruit[1][0] = 31;
  fruit[1][1] = 30;
  fruit[2][0] = 31;
  fruit[2][1] = 31;
  fruit[3][0] = 30;
  fruit[3][1] = 31;
  
  grid[30][30] = 2;
  grid[31][30] = 2;
  grid[31][31] = 2;
  grid[30][31] = 2;
  
}

void gameOver() {
  printf("LOSER LOL");
}

int checkCollisions() {
  node_t* head = list_get_first();
  node_t* curr = list_get_first();

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

}

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
  else {
    dir[0] = 0;
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
    gameOver();
  }
  
  if ((first->y + dir[1]) >= 0 && (first->y + dir[1]) < height){
    newNode->y = first->y + dir[1];
  }
  else {
    // we hit a wall
    gameOver();
  }

  list_insert(newNode);

/*
  // check if we eat a fruit
  if (newNode->x == fruitx && newNode->y == fruity){
    score += 1;
    // generate a new fruit
    generateFruit();
    // don't pop the tail
  }
  
  else {
  */
    // pop tail of snake
    node_t* last = list_remove_last();
    free(last);
  //}
  
  // check for collisions
  if(checkCollisions()) {
    // we hit ourselves
    gameOver();
  }
  
  
}
