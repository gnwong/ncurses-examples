/*
 *  Conway's Game of Life
 *
 *  Copyright (c) 2014  George Wong
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see http://www.gnu.org/licenses/.
 *
 */

#include <time.h>
#include <stdlib.h>
#include <ncurses.h>

void randomify (int *change, int rows, int cols);
void progenify (bool *world, int *change, int rows, int cols);

int main () {

  int rows;
  int cols;
  int world_width;
  int world_height;
  bool *world;
  int *change;
  int pop1, pop2;
  time_t rawtime;
  struct tm * timeinfo;
  char in;
  int TIMEOUT = 50;

  // Init ncurses and get window info
  initscr();
  getmaxyx(stdscr,rows,cols);
  timeout(TIMEOUT);

  // Draw border
  for (int i=0; i<cols; i++) {
    mvaddch(0,i,'=');
    mvaddch(rows-1,i,'=');
  }
  for (int i=1; i<rows-1; i++) {
    mvaddch(i,0,'|');
    mvaddch(i,cols-1,'|');
  }

  // Randomly generate starting map
  world = calloc(rows*cols, sizeof(bool));
  change = calloc(rows*cols, sizeof(int));
  randomify(change, rows, cols);
  
  // Main loop
  while (true) {

    // Update time
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    mvprintw(rows-2,cols-26,"%s", asctime(timeinfo) );
    mvprintw(rows-2,2,"%d  ",pop2);
    mvaddch(rows-2,cols-1,'|');

    // Determine whether to randomize
    pop1 = pop2;
    pop2 = 0;

    // Update the playing field    
    for (int i=0; i<rows; i++) {
      if (i<2 || i>(rows-3)) continue;
      for (int j=0; j<cols; j++) {
        if (j<2 || j>(cols-3)) continue;
        // j is column (i.e. x)

        if (world[i*cols+j]) pop2++;

        if (change[i*cols+j] != 0) {
          if (change[i*cols+j] == -1) {
            world[i*cols+j] = 0;
          } else if (change[i*cols+j] == 1) {
            world[i*cols+j] = 1;  
          }
          
          if (world[i*cols+j]) {
            mvaddch(i,j,'*');
          } else {
            mvaddch(i,j,' ');
          }
        }
      }
    }

    // Make the cursor move to an unassuming position and print
    move(rows-1,cols-1);
    refresh();

    // Advance!
    progenify(world, change, rows, cols);

    // Pause / deal with input
    in = getch();
    if (in == 'q') break;
    if (in == 'r') randomify(change, rows, cols);
    if (in == 'p') timeout(-1);
    if (in == 'c') timeout(TIMEOUT);
  }

  // House keeping
  free(world);
  endwin();

  return 0;
}

// Gets a random starting distribution for the world
void randomify (int *change, int rows, int cols) {
  for (int i=0; i<rows*cols; i++) {
    if (rand() / (double)RAND_MAX > 0.8) {
      change[i] = 1;
    } else {
      change[i] = 0;
    }
  }
}

// Clotho and Atropos
void progenify (bool *world, int *change, int rows, int cols) {

  int neighbours;

  for (int i=1; i<rows-2; i++) {
    for (int j=1; j<cols-2; j++) {
      neighbours = 0;
      if (world[(i-1)*cols+j-1])  neighbours++;
      if (world[(i-1)*cols+j])    neighbours++;
      if (world[(i-1)*cols+j+1])  neighbours++;
      if (world[i*cols+j-1])      neighbours++;
      if (world[i*cols+j+1])      neighbours++;
      if (world[(i+1)*cols+j-1])  neighbours++;
      if (world[(i+1)*cols+j])    neighbours++;
      if (world[(i+1)*cols+j+1])  neighbours++;

      // Default action
      change[i*cols+j] = 0;

      // Any live cell with fewer than two live neighbours dies, as if caused by under-population.
      if (neighbours < 2) {
        change[i*cols+j] = -1;
      }

      // Any live cell with more than three live neighbours dies, as if by overcrowding.
      if (neighbours > 3) {
        change[i*cols+j] = -1;
      }

      // Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction
      if (neighbours == 3) {
        change[i*cols+j] = 1;
      } 
    }
  }

}
