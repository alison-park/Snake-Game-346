#include <stdlib.h>
//#include <conio.h>
#include <stdio.h>
#include <linked_list.h>

int i,j,height = 16, width = 32, score;
int fruitx,fruity;
int dir[2] = {1, 0};
int grid[32][16] = {0};

void update_grid(){
  // clear board + update it based on linked list
  for(i=0; i< width; i++){
    for(j=0; j< height; j++){
      grid[i][j] = 0;
    }
  }
  node_t* first=list_get_first();
  while(first!=NULL){
    grid[first->y][first->x]=1;
    first=first->next;
  }
}

void draw(){
  update_grid();
  printf("/nDrawing the grid");
  for (i=0; i<width; i++){ //column, toggle to turn on column
    printf("/nColumn %d:", i);
    for (j=0; j<height; j++){ //row, toggle to turn on specific LED
      //set led to value of grid[width][height]
      printf(" %d", grid[i][j]);
    }
  }
}

void setup(){
  node_t* threeNode = malloc(sizeof(node_t));
  threeNode->x = 0;
  threeNode->y = 0;
  list_insert(threeNode);
  
  node_t* twoNode = malloc(sizeof(node_t));
  twoNode->x = 1;
  twoNode->y = 0;
  list_insert(twoNode);
  
  node_t* oneNode = malloc(sizeof(node_t));
  oneNode->x = 2;
  oneNode->y = 0;
  list_insert(oneNode);

  
  // generate fruit positions
  
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

  // check if we eat a fruit
  if (newNode->x == fruitx && newNode->y == fruity){
    score += 1;
    // generate a new fruit
    generateFruit();
    // don't pop the tail
  }
  else {
    // pop tail of snake
    node_t* last = list_remove_last();
    free(last);
  }
  
  // check for collisions
  if(checkCollisions()) {
    // we hit ourselves
    gameOver();
  }
  
  
}

void input(){

}
