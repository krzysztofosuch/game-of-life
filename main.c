#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

struct Creature {
  int alive;
};
int coordinatesToIndex(int width, int x, int y) {
  return width*y+x;
}
unsigned int getConsoleInput(char **pStrBfr) //pass in pointer to char pointer, returns size of buffer
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
        //on realloc error, NULL is returned but original buffer is unchanged
        //NOTE: the buffer WILL NOT be NULL terminated since last
        //chracter came from console
        if(strbfr==NULL) goto error;
    }
    strbfr[i] = '\0';
    *pStrBfr = strbfr; //successfully returns pointer to NULL terminated buffer
    return i + 1; 
    error:
    *pStrBfr = strbfr;
    return i + 1;
}
void printMap(struct Creature* creatures, int width, int height) {
  printf("x");
  for(int y=0;y<height;y++) {
    printf("x");
  }
  printf("x\n");
  for(int x=0;x<width;x++) {
    printf("x");
    for(int y=0;y<height;y++) {
      printf("%d", creatures[coordinatesToIndex(width, x,y)].alive);
    }
    printf("x\n");
  }
  printf("x");
  for(int y=0;y<height;y++) {
    printf("x");
  }
  printf("x\n");
}
int countAliveNeighbors(struct Creature* creatures, int width, int height, int x, int y) {
  int count = 0;
  if (x > 1) {
    count += creatures[coordinatesToIndex(width, x-1, y)].alive;
  }
  if (x < width-1) {
    count += creatures[coordinatesToIndex(width, x+1, y)].alive;
  }
  if (y > 1) {
    count += creatures[coordinatesToIndex(width, x, y-1)].alive;
  }
  if (y < height-1) {
    count += creatures[coordinatesToIndex(width, x, y+1)].alive;
  }
  // diagonal
  if (x > 1 && y > 1) {
    count += creatures[coordinatesToIndex(width, x-1, y-1)].alive;
  }
  if (x > 1 && y < height-1) {
    count += creatures[coordinatesToIndex(width, x-1, y+1)].alive;
  }
  if (x < width-1 && y > 1) {
    count += creatures[coordinatesToIndex(width, x+1, y-1)].alive;
  }
  if (x < width-1 && y < height-1) {
    count += creatures[coordinatesToIndex(width, x+1, y+1)].alive;
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
  struct Creature *creatures;
  creatures = (struct Creature*)malloc(sizeof(struct Creature) * width * height);
  for(int x=0;x<width;x++) {
    for(int y=0;y<height;y++) {
      creatures[coordinatesToIndex(width, x,y)].alive = 0;
    }
  }
  printMap(creatures, width, height);
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
			for(int i = 0; i<len;i++) {
			  if (isdigit(line[i])) {
          buffer[cursor++] = line[i];
        } else {
          buffer[cursor] = 0x00;
          cursor=0;
          if (!x_set) {
            x = atoi(buffer);
            x_set = 1;
          } else if (!y_set) {
            y = atoi(buffer);
            y_set = 1;
          } else {
            printf("INVALID INPUT 1\n");
            memset(buffer, 0, len);
          }
        }
			}
      if (x_set && y_set && x<width && y<height) {
        struct Creature* creatureAtField = &creatures[coordinatesToIndex(width, x, y)];
        (*creatureAtField).alive = (*creatureAtField).alive ? 0 : 1;
        printMap(creatures, width, height);
      } else {
        printf("INVALID INPUT 2: x:%d, y:%d, w:%d, h:%d\n", x, y, width, height);
      }
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
        int aliveNeighbors = countAliveNeighbors(creatures, width, height, x, y);
        int alive = creatures_next[coordinatesToIndex(width, x, y)].alive;
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
    creatures = creatures_next;
    printMap(creatures, width, height);
    getchar();
  }
}

