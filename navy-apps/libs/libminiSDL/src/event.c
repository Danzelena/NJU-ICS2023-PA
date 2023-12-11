#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

struct __event_element{
  uint8_t type;
  uint8_t sym;
  struct __event_element* next;
};
typedef struct __event_element event_element;

static event_element event_queue = {.type = 0, .sym = 0, .next = NULL};
static event_element *end = &event_queue;

static void append(uint8_t type, uint8_t sym){
  event_element *new_element = malloc(sizeof(event_element));
  new_element->type = type;
  new_element->sym = sym;
  new_element->next = NULL;
  end->next = new_element;
  end = new_element;
}

static int pop(uint8_t *type, uint8_t *sym){
  if (event_queue.next == NULL){
    return 0;
  }else {
    event_element *buf = event_queue.next;
    *type = buf->type;
    *sym = buf->sym;
    event_queue.next = buf->next;
    if (buf == end){
      end = &event_queue;
    }
    free (buf);
  }
  return 1;
}

static uint8_t key_state[sizeof(keyname) / sizeof(keyname[0])] = {0};

// void SDL_PumpEvents(void){
//   assert(0);
//   char buf[64], action[8], key[32];
  
//   //printf("Doing PumpEvent...\n");
//   int ret;
//   uint8_t type = 0, sym = 0;
//   while ((ret = NDL_PollEvent(buf, sizeof(buf)), ret)){
//     sscanf(buf, "%s %s", action, key);

//     for (int i = 0; i < sizeof(keyname) / sizeof(keyname[0]); ++i){
//       if (strcmp(key, keyname[i]) == 0){
//         sym = i;
//         break ;
//       }
//     }

//     if (strcmp("kd", action) == 0){
//       type = SDL_KEYDOWN;
//     }else if(strcmp("ku", action) == 0){
//       type = SDL_KEYUP;
//     }else {
//       assert(0);
//     }

//     append(type, sym);
//   }
// }


int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

static char key_buf[64], *key_action, *key_key;
//To Be Fast
static int inline read_keyinfo(uint8_t *type, uint8_t *sym){
  int ret = NDL_PollEvent(key_buf, sizeof(key_buf));
  if (!ret){
    return 0;
  }
  //printf("%s\n", key_buf);
  key_action = key_buf;
  int i;
  for (i = 0; key_buf[i] != ' '; i++){}
  key_buf[i] = '\0';
  key_key = &key_buf[i + 1]; 
  
  //截断\n
  for (i = 0;  key_key[i] != '\0' && key_key[i] != '\n'; i++){}
  if (key_key[i] == '\n'){
    key_key[i] = '\0';
  }
  
  //strcmp("kd", key_action) == 0
  if (key_action[1] == 'd'){//加速！！
    *type = SDL_KEYDOWN;
  }else{
    *type = SDL_KEYUP;
  }

  for (i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
    //剪枝掉很多
    if (key_key[0] == keyname[i][0] && strcmp(key_key, keyname[i]) == 0){
      *sym = i;
      //printf("%d %d\n", *type, *sym);
      return ret;
    }
  }
}

int SDL_PollEvent(SDL_Event *ev) {
  uint8_t type = 0, sym = 0;
  // SDL_PumpEvents();


  // if (pop(&type, &sym)){
  //printf("SDL_PollEvent\n");
  if (read_keyinfo(&type, &sym)){
    ev->type = type;
    ev->key.keysym.sym = sym;

    switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      //printf("%d Down\n", (int)sym);
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      //printf("%d Up\n", (int)sym);
      break;
    }
  }else {
    return 0;
  }

  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
  uint8_t type = 0, sym = 0;
  //SDL_PumpEvents();

  //while (!pop(&type, &sym)){
  while (!read_keyinfo(&type, &sym)){
    //SDL_PumpEvents();
  }
  
  event->type = type;
  event->key.keysym.sym = sym;

  switch(type){
    case SDL_KEYDOWN:
      key_state[sym] = 1;
      break;
    
    case SDL_KEYUP:
      key_state[sym] = 0;
      break;
  }

  // char buf[64], action[8], key[32];
  
  // while (!NDL_PollEvent(buf, sizeof(buf))) {}
  
  // //注意：这里代码和Poll中重复
  // sscanf(buf, "%s %s", action, key);
  
  // if (strcmp("kd", action) == 0){
  //   event->type = SDL_KEYDOWN;
  // }else if(strcmp("ku", action) == 0){
  //   event->type = SDL_KEYUP;
  // }else {
  //   assert(0);
  // }

  // for (int i = 0; i < sizeof(keyname) / sizeof(char *); ++i){
  //   if (strcmp(key, keyname[i]) == 0){
  //     event->key.keysym.sym = i;
  //     break ;
  //   }
  // }

  // return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;

  if (numkeys)
    *numkeys = sizeof(key_state) / sizeof(key_state[0]);
  //SDL_PumpEvents();
  return key_state;
}
// #include <NDL.h>
// #include <SDL.h>

// #define keyname(k) #k,
// #define RED_YEL "\033[31;43m"
// #define WRITE "\033[0m"

// static const char *keyname[] = {
//   "NONE",
//   _KEYS(keyname)
// };

// int SDL_PushEvent(SDL_Event *ev) {
//   printf(RED_YEL"(1)NO implement!\n"WRITE);
// 	exit(-1);
//   return 0;
// }

// int SDL_PollEvent(SDL_Event *ev) {
//   printf(RED_YEL"(2)NO implement!\n"WRITE);
// 	exit(-1);
//   return 0;
// }

// int SDL_WaitEvent(SDL_Event *event) {
//   char buf[64];
//   while(1){
//     if(NDL_PollEvent(buf,sizeof(buf))){
//     //BUG:only handle signle line case
//     char dir;
//     char key[16];
//     sscanf(buf,"k%c %s",&dir,key);
    
//     if (dir == 'd'){
//       event->type = SDL_KEYDOWN;
//       printf("down!\n");
//       printf("KEY=%s\n",key);

//       if(strcmp(key,"DOWN") == 0){
//         event->key.keysym.sym = SDLK_DOWN;
//       }else if(strcmp(key,"UP") == 0){
//         event->key.keysym.sym = SDLK_UP;
//         printf("SDLK_UP\n");
//       }else if(strcmp(key,"RIGHT") == 0){
//         event->key.keysym.sym = SDLK_RIGHT;
//         printf("SDLK_RIGHT\n");
//       }else if(strcmp(key,"LEFT") == 0){
//         event->key.keysym.sym = SDLK_LEFT;
//         printf("SDLK_LEFT\n");
//       }
//       else{
//         // BUG: don't know how to set status
//         event->key.keysym.sym = SDLK_NONE;
//       }
    
//     }else if(dir == 'u'){
//       event->type = SDL_KEYUP;
//     }else{
//       printf("There can't be another char after 'k'\n");
//     }
//     printf("type=%d,sym=%d\n",event->type,event->key.keysym.sym);
//     break;

//   }else{
//     // event->type = -1;
//     // event->key.keysym.sym = SDLK_NONE;
//   }
//   }
  
//   // printf("type=%d,sym=%d\n",event->type,event->key.keysym.sym);
//   // printf(RED_YEL"(3)NO implement!\n"WRITE);
// 	// exit(-1);
//   printf("return\n");
//   return 1;
// }

// int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
//   printf(RED_YEL"(4)NO implement!\n"WRITE);
// 	exit(-1);
//   return 0;
// }

// uint8_t* SDL_GetKeyState(int *numkeys) {
//   printf(RED_YEL"(5)NO implement!\n"WRITE);
// 	exit(-1);
//   return NULL;
// }
