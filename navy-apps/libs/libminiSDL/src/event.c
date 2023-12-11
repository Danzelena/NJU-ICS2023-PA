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
  printf(RED_YEL"(1)NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  printf(RED_YEL"(2)NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64];
  if(NDL_PollEvent(buf,sizeof(buf))){
    //BUG:only handle signle line case
    char dir;
    char key[16];
    sscanf(buf,"k%c %s",&dir,key);
    
    if (dir == 'd'){
      event->type = SDL_KEYDOWN;
      printf("down!\n");
      printf("KEY=%s\n",key);

      if(strcmp(key,"DOWN") == 0){
        event->key.keysym.sym = SDLK_DOWN;
      }else if(strcmp(key,"UP") == 0){
        event->key.keysym.sym = SDLK_UP;
        printf("SDLK_UP\n");
      }else if(strcmp(key,"RIGHT") == 0){
        event->key.keysym.sym = SDLK_RIGHT;
      }else if(strcmp(key,"LEFT") == 0){
        event->key.keysym.sym = SDLK_LEFT;
      }
      else{
        // BUG: don't know how to set status
        event->key.keysym.sym = SDLK_NONE;
      }
    
    }else if(dir == 'u'){
      event->type = SDL_KEYUP;
    }else{
      printf("There can't be another char after 'k'\n");
    }

  }else{
    event->key.keysym.sym = SDLK_NONE;
  }
  // printf(RED_YEL"(3)NO implement!\n"WRITE);
	// exit(-1);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  printf(RED_YEL"(4)NO implement!\n"WRITE);
	exit(-1);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  printf(RED_YEL"(5)NO implement!\n"WRITE);
	exit(-1);
  return NULL;
}
