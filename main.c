#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <time.h> 

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
  printf("x");
  for(int y=0;y<(*map).height;y++) {
    printf("x");
  }
  printf("x\n");
  for(int x=0;x<(*map).width;x++) {
    printf("x");
    for(int y=0;y<(*map).height;y++) {
      printf("%c", (*map).creatures[coordinatesToIndex((*map).width, x,y)].alive ? '0' : ' ');
    }
    printf("x\n");
  }
  printf("x");
  for(int y=0;y<(*map).height;y++) {
    printf("x");
  }
  printf("x\n");
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
  if (argc != 3) {
    printf("required 2 args, %d given\n", argc-1);
    return 1;
  }
  int height = atoi(argv[1]);
  int width = atoi(argv[2]);
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
  int lineWasEmpty = 0;
	do {
		char* line;
		int len = getConsoleInput(&line);
    char* buffer = malloc(len);
		if (len > 1) {
      int cursor = 0;
      int x=0;
      int y=0;
      int x_set = 0;
      int y_set = 0;
      if (line[0] == 'r' || line[0] == 'R') {
        time_t seed;
        time(&seed);
        srand(seed);
        for(int x=0;x<width;x++) {
          for(int y=0;y<height;y++) {
            map.creatures[coordinatesToIndex(width, x,y)].alive = (rand() < (RAND_MAX/3) ? 1 : 2);
          }
        }
      } else {
        for(int i = 0; i<len;i++) {
          if (isdigit(line[i])) {
            buffer[cursor++] = line[i];
          } else {
            buffer[cursor] = 0x00;
            cursor=0;
            if (!x_set) {
              if (strlen(buffer) > 0) {
                x = atoi(buffer);
                x_set = 1;
              } else {
                printf("INVALID INPUT X\n");
                break;
              }
            } else if (!y_set) {
              if (strlen(buffer) > 0) {
                y = atoi(buffer);
                y_set = 1;
              } else {
                printf("INVALID INPUT Y\n");
                break;
              }
            } else {
              printf("INVALID INPUT 1\n");
              memset(buffer, 0, len);
            }
          }
        }
        if (x_set && y_set && x<width && y<height) {
          struct Creature* creatureAtField = &map.creatures[coordinatesToIndex(width, x, y)];
          (*creatureAtField).alive = (*creatureAtField).alive ? 0 : 1;
        } else {
          printf("INVALID INPUT 2: x:%d, y:%d, w:%d, h:%d\n", x, y, width, height);
        }
      }
      printMap(&map);
		} else {
      printf("Line was empty\n");
			lineWasEmpty = 1;
		}
    free(buffer);
	} while(lineWasEmpty == 0);
  struct Creature *creatures_next = (struct Creature*)malloc(sizeof(struct Creature) * width * height);
  while(1) {
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
    char input = getchar();
    if (input == 'q' || input == 'Q') {
      break;
    }
  }
  free(map.creatures);
  free(creatures_next);
}

