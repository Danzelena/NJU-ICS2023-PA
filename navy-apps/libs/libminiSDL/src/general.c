#include <NDL.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

int SDL_Init(uint32_t flags) {
  printf("SDL init\n");
  return NDL_Init(flags);
}

// Warning:after SDL_Quit(), will go back to os face
void SDL_Quit() {
  // noexistBUG: Segementation fault
  NDL_Quit();
  // printf(RED_YEL"(6)NO implement!\n"WRITE);
	// exit(-1);
}

char *SDL_GetError() {
  printf(RED_YEL"(7)NO implement!\n"WRITE);
	exit(-1);
  return "Navy does not support SDL_GetError()";
}

int SDL_SetError(const char* fmt, ...) {
  printf(RED_YEL"(8)NO implement!\n"WRITE);
	exit(-1);
  return -1;
}

int SDL_ShowCursor(int toggle) {
  printf(RED_YEL"(9)NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

void SDL_WM_SetCaption(const char *title, const char *icon) {

  printf(RED_YEL"(10)NO implement!\n"WRITE);
	// exit(-1);
}
