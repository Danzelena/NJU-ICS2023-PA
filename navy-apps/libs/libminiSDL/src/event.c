#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,
#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return NULL;
}
