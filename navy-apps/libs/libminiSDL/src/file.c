#include <sdl-file.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

SDL_RWops* SDL_RWFromFile(const char *filename, const char *mode) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return NULL;
}

SDL_RWops* SDL_RWFromMem(void *mem, int size) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return NULL;
}
