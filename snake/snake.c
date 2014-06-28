/*
 *  Snake Game
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
#include <string.h>


int game ();
int print_menu (int rows, int cols);
int get_options (int rows, int cols);

int main () {

  int rows, cols;

  // Init
  initscr();
  getmaxyx(stdscr, rows, cols);

  endwin();

  _LOOP:

  // Show menu
  switch (print_menu(rows, cols)) {
    case 1:
      game();
      break;

    case 2:
      get_options(rows, cols);
      break;

    default:
      goto _END;
  }

  goto _LOOP;

  // House keeping
  _END:

  endwin();
  return 0;
}


int game () {
  erase();

  char in;

  int rows, cols;
  getmaxyx(stdscr,rows,cols);
  timeout(100);

  int tracer[2] = {rows/2, cols/3};
  int length = 2;
  bool growing = false;

  int direction = 2;
  // 0 : left
  // 1 : down
  // 2 : right
  // 3 : up

  // Make the world
  int *world;
  world = calloc(rows*cols, sizeof(int));
  if (world == NULL) return -1;

  // Draw border
  for (int i=0; i<cols; i++) {
    world[i] = -1;
    world[(rows-1)*cols+i] = -1;
  }
  for (int i=1; i<rows-1; i++) {
    world[i*cols] = -2;
    world[i*cols+cols-1] = -2;
  }

  // Place ambrosia
  do {
    int ambrosia = rand() % (rows*cols);
    if (world[ambrosia] == 0) {
      world[ambrosia] = -5;
      break;
    }
  } while (true);

  // Loop
  while (true) {

    // Advance the snake
    switch (direction) {
      case 0:
        tracer[1]--;
        break;
      case 1:
        tracer[0]++;
        break;
      case 2:
        tracer[1]++;
        break;
      default:
        tracer[0]--;
    }

    // Verify no collisions
    if (world[tracer[0]*cols+tracer[1]] != 0) {
      if (world[tracer[0]*cols+tracer[1]] == -5) {
        // Place new ambrosia
        do {
          int ambrosia = rand() % (rows*cols);
          if (world[ambrosia] == 0) {
            world[ambrosia] = -5;
            break;
          }
        } while (true);

        // Update the snake
        length++;
        growing = true;

      } else { 
        timeout(-1);
        getch();
        break;
      }
    }

    // Draw the snake
    world[tracer[0]*cols+tracer[1]] = length;

    // Draw map
    for (int i=0; i<rows; i++) {
      for (int j=0; j<cols; j++) {

          if (world[i*cols+j] == 0) mvaddch(i,j,' ');
          else if (world[i*cols+j] > 0) {
            mvaddch(i,j,'#');
            if (!growing) world[i*cols+j]--;
          }
          else if (world[i*cols+j] == -1) mvaddch(i,j,'=');
          else if (world[i*cols+j] == -2) mvaddch(i,j,'|');
          else if (world[i*cols+j] == -3) mvaddch(i,j,'.');
          else if (world[i*cols+j] == -5) mvaddch(i,j,'*');
          else mvaddch(i,j,'.');
      }
    }

    growing = false;

    // Move cursor appropriately
    move(rows-1,cols-1);

    // Handle arrow keys
    if (getch() == '\033') {
      getch();
      switch(getch()) {
        case 'A':
          if (direction==1) break;
          direction=3;
          break;
        case 'B':
          if (direction==3) break;
          direction=1;
          break;
        case 'C':
          if (direction==0) break;
          direction=2;
          break;
        case 'D':
          if (direction==2) break;
          direction=0;
          break;
      }
    }
    mvaddch(rows-1,cols-1,'=');

  }

  timeout(-1);
  free(world);

  return 0;
}

// TODO
int get_options (int rows, int cols) {
  erase();

  mvprintw(rows/2-3, (cols-14)/2, "Not yet implemented...");

  getch();

  return 0;
}

// Prints main menu & gets input
int print_menu (int rows, int cols) {
  erase();

  char in;
  int choice=3;

  mvprintw(rows/2-6, (cols-14)/2, "   Snake Game");
  mvprintw(rows/2-5, (cols-14)/2, "================");
  mvprintw(rows/2-4, (cols-14)/2, " 1.  New Game");
  mvprintw(rows/2-3, (cols-14)/2, " 2.  Options");
  mvprintw(rows/2-2, (cols-14)/2, " 3.  Quit");
  mvprintw(rows/2, (cols-14)/2, "input > 3");

  while (true) {
    in = getch();
    if (in == 10) {
      return choice;
    }
    if (in=='1' || in=='2' || in=='3') {
      mvaddch(rows/2, (cols+2)/2, in);
      if (in=='1') choice = 1;
      if (in=='2') choice = 2;
      if (in=='3') choice = 3;
    }
    mvaddch(rows/2, (cols+4)/2, ' ');
    mvaddch(rows/2, (cols+6)/2, ' ');
    mvaddch(rows/2, (cols+8)/2, ' ');
    move(rows/2, (cols+4)/2);
  }
}

