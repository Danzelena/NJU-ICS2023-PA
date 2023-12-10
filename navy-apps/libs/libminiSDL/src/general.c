#include <NDL.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

int SDL_Init(uint32_t flags) {
  return NDL_Init(flags);
}

void SDL_Quit() {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  NDL_Quit();
}

char *SDL_GetError() {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return -1;
}

int SDL_ShowCursor(int toggle) {
  printf(RED_YEL"()NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {
  printf(RED_YEL"(fixedpt_ceil)NO implement!\n"WRITE);
	exit(-1);
}
