#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h> 
#include <ncurses.h> 

struct Creature {
  int alive;
};
struct Map {
  struct Creature* creatures;
  int width;
  int height;
};
int coordinatesToIndex(int width, int x, int y) {
  return width*y+x;
}
unsigned int getConsoleInput(char **pStrBfr) 
{
    char * strbfr;
    int c;
    unsigned int i;
    i = 0;
    strbfr = (char*)malloc(sizeof(char));
    if(strbfr==NULL) goto error;
    while( (c = getchar()) != '\n' && c != EOF )
    {
        strbfr[i] = (char)c;
        i++;
        strbfr = (void*)realloc((void*)strbfr,sizeof(char)*(i+1));
        if(strbfr==NULL) goto error;
    }
    strbfr[i] = '\0';
    *pStrBfr = strbfr;
    return i + 1; 
    error:
    *pStrBfr = strbfr;
    return i + 1;
}
void printMap(struct Map* map) {
  char character = ' ';
  for(int x=0;x<(*map).width;x++) {
    for(int y=0;y<(*map).height;y++) {
      character = (*map).creatures[coordinatesToIndex((*map).width, x,y)].alive ? '#' : ' ';
      // printf("%d %d %s" , x,y, &character);
      mvprintw(x,y,&character);
    }
  }
  refresh();
}
int countAliveNeighbors(struct Map* map, int x, int y) {
  int count = 0;
  int width = (*map).width;
  int height = (*map).height;
  if (x > 0) {
    count += (*map).creatures[coordinatesToIndex((*map).width, x-1, y)].alive;
  } 
  if (x < width-1) {
    count += (*map).creatures[coordinatesToIndex(width, x+1, y)].alive;
  }
  if (y > 0) {
    count += (*map).creatures[coordinatesToIndex(width, x, y-1)].alive;
  }
  if (y < height-1) {
    count += (*map).creatures[coordinatesToIndex(width, x, y+1)].alive;
  }
  //diagonal
  if (x > 0 && y > 0) {
    count += (*map).creatures[coordinatesToIndex(width, x-1, y-1)].alive;
  }
  if (x > 0 && y < height-0) {
    count += (*map).creatures[coordinatesToIndex(width, x-1, y+1)].alive;
  }
  if (x < width-0 && y > 0) {
    count += (*map).creatures[coordinatesToIndex(width, x+1, y-1)].alive;
  }
  if (x < width-0 && y < height-0) {
    count += (*map).creatures[coordinatesToIndex(width, x+1, y+1)].alive;
  }
  return count;
}
int main(int argc, char* argv[]) {
  initscr();
  int timeout = 50;
  int height; 
  int width;
  getmaxyx(stdscr, width, height);
  keypad(stdscr, TRUE);
  // printf("%d %d", width, height);
  struct Map map;
  map.width = width;
  map.height = height;
  map.creatures = (struct Creature*)malloc(sizeof(struct Creature) * width * height);
  for(int x=0;x<width;x++) {
    for(int y=0;y<height;y++) {
      map.creatures[coordinatesToIndex(width, x,y)].alive = 0;
    }
  }
  printMap(&map);
  time_t seed;
  time(&seed);
  srand(seed);
  for(int x=0;x<width;x++) {
    for(int y=0;y<height;y++) {
      map.creatures[coordinatesToIndex(width, x,y)].alive = (rand() < (RAND_MAX/3) ? 0 : 1);
    }
  }
  struct Creature *creatures_next = (struct Creature*)malloc(sizeof(struct Creature) * width * height);
  while(1) {
    timeout(timeout);
    for(int x=0;x<width;x++) {
      for(int y=0;y<height;y++) {
        int aliveNeighbors = countAliveNeighbors(&map, x, y);
        int alive = map.creatures[coordinatesToIndex(width, x, y)].alive;
        if (alive == 0) {
          if (aliveNeighbors == 3) {
            alive = 1;
          } 
        } else {
          if (aliveNeighbors < 2) {
            alive = 0;
          } else if (aliveNeighbors > 3) {
            alive = 0;
          }
        }
        creatures_next[coordinatesToIndex(width, x, y)].alive = alive;
      }
    }
    struct Creature* tmp = map.creatures;
    map.creatures = creatures_next;
    creatures_next = tmp;
    printMap(&map);
    char input = getch();
    if (input == KEY_UP) {
      timeout += 100;
    }
    if (input == KEY_DOWN) {
      if (timeout > 0) {
        timeout -= 100;
      }
    }
    if (input == 'q' || input == 'Q') {
      break;
    }
  }
  free(map.creatures);
  free(creatures_next);
  endwin();
  return 0;
}

