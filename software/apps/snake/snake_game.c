#include <stdlib.h>
#include <conio.h>
#include <stdio.h>i
#include <linked_list.h>

int i,j,height = 16, width = 32, gameover, score;
int x,y,fruitx,fruity,flag;
int grid[width][height] = {0};

void update_grid(){
  // clear board + update it based on linked list
  int clear_grid[width][height]={0};
  grid=clear_grid;
  node_t* first=list_get_first();
  while(first!=NULL){
    grid[first.y][first.x]=1;
    first=first.next;
  }
}

void draw(){
  update_grid();
  for (i=0; i<width; i++){ //column, toggle to turn on column
    for (j=0; j<height; j++){ //row, toggle to turn on specific LED
      //set led to value of grid[width][height]
    }
  }
}
void setup(){
}

void logic(){
}

void input(){

}
