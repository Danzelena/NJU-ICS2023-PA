#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  printf(RED_YEL"(11)NO implement!\n"WRITE);
	exit(-1);
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  printf(RED_YEL"(12)NO implement!\n"WRITE);
	exit(-1);
  return 1;
}


// Get the number of milliseconds since the SDL library initialization.
uint32_t SDL_GetTicks() {
  return NDL_GetTicks();
  // printf(RED_YEL"(13)NO implement!\n"WRITE);
	// exit(-1);
  // return 0;
}

void SDL_Delay(uint32_t ms) {
  uint32_t start = NDL_GetTicks();
  while (NDL_GetTicks() - start <= ms){}
  // printf(RED_YEL"(14)NO implement!\n"WRITE);
	// exit(-1);
}
