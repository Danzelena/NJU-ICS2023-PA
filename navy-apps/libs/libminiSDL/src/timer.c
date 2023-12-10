#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  printf(RED_YEL"(fixedpt_ceil)NO implement!\n"WRITE);
	exit(-1);
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  printf(RED_YEL"(fixedpt_ceil)NO implement!\n"WRITE);
	exit(-1);
  return 1;
}

uint32_t SDL_GetTicks() {
  printf(RED_YEL"(fixedpt_ceil)NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

void SDL_Delay(uint32_t ms) {
  printf(RED_YEL"(fixedpt_ceil)NO implement!\n"WRITE);
	exit(-1);
}
