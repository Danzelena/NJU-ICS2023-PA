
#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,
#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};

static uint8_t key_state[sizeof(keyname) / sizeof(keyname[0])] = {0};

int SDL_PushEvent(SDL_Event *ev)
{
  printf(RED_YEL "(1)NO implement!\n" WRITE);
  exit(-1);
  return 0;
}
bool isSignLine(char *str)
{
  bool newLineFound = false;
  while (*str)
  {
    if (*str == '\n' || *str == '\r')
    {
      if (newLineFound)
      {
        return false;
      }
      newLineFound = true;
      str++;
      if (*str != '\0' && *str != '\n' && *str != '\r')
      {
        return false;
      }
    }
    else
    {
      str++;
    }
  }
  return true;
}
int SDL_PollEvent(SDL_Event *ev)
{
  char buf[64];
  if (!NDL_PollEvent(buf, sizeof(buf)))
  {
    return 0;
  }
  // while (NDL_PollEvent(buf, sizeof(buf))){};
  // printf("key down\n");
  // while (!NDL_PollEvent(buf, sizeof(buf)));
  // printf("pass %s\n",buf);
  // assert signle line
  if (!isSignLine(buf))
  {
    printf(RED_YEL "(SDL_PollEvent)buf is not signle line!\n" WRITE);
    exit(-1);
  }
  char dir;
  char key[16];
  sscanf(buf, "k%c %s", &dir, key);

  bool down = false;
  if (dir == 'd')
  {
    ev->type = SDL_KEYDOWN;
    down = true;
  }
  else if (dir == 'u')
  {
    ev->type = SDL_KEYUP;
    down = false;
  }
  else
  {
    printf(RED_YEL "(SDL_PollEvent)no another case!\n" WRITE);
    exit(-1);
  }

  for (int i = 0; i < sizeof(keyname) / sizeof(char); i++)
  {
    // printf("key=%s,keyname[i]=%s\n",key,keyname[i]);
    if (strcmp(key, keyname[i]) == 0)
    {
      // printf("event:%s,sym:%d\n",key,i);
      ev->key.keysym.sym = i;
      key_state[i] = down ? 1 : 0;
      return 1;
    }
  }
  return 0;
  // return 1;
  // printf(RED_YEL "(2)NO implement!\n" WRITE);
  // exit(-1);
  // return 0;
}

// Waits indefinitely for the next available event, returning 1, or 0 if there was an error while waiting for events.
int SDL_WaitEvent(SDL_Event *event)
{
  char buf[64];
  while (1)
  {
    if (NDL_PollEvent(buf, sizeof(buf)))
    {
      // assert only handle signle line case
      if (!isSignLine(buf))
      {
        printf(RED_YEL "(SDL_WaitEvent)buf is not signle line!\n" WRITE);
        exit(-1);
      }
      char dir;
      char key[16];
      sscanf(buf, "k%c %s", &dir, key);

      if (dir == 'd')
      {
        event->type = SDL_KEYDOWN;
        // printf("down!\n");
        // printf("KEY=%s\n", key);
        for (int i = 0; i < sizeof(keyname) / sizeof(char); i++)
        {
          if (strcmp(key, keyname[i]) == 0)
          {
            key_state[i] = 1;
                  printf("(WaitEvent)key=%s\n",key);

            event->key.keysym.sym = i;
            break;
          }
        }
        // if (strcmp(key, "DOWN") == 0)
        // {
        //   event->key.keysym.sym = SDLK_DOWN;
        // }
        // else if (strcmp(key, "UP") == 0)
        // {
        //   event->key.keysym.sym = SDLK_UP;
        //   printf("SDLK_UP\n");
        // }
        // else if (strcmp(key, "RIGHT") == 0)
        // {
        //   event->key.keysym.sym = SDLK_RIGHT;
        //   printf("SDLK_RIGHT\n");
        // }
        // else if (strcmp(key, "LEFT") == 0)
        // {
        //   event->key.keysym.sym = SDLK_LEFT;
        //   printf("SDLK_LEFT\n");
        // }
        // else
        // {
        //   event->key.keysym.sym = SDLK_NONE;
        // }
      }
      else if (dir == 'u')
      {
        event->type = SDL_KEYUP;
        for (int i = 0; i < sizeof(keyname) / sizeof(char); i++)
        {
          if (strcmp(key, keyname[i]) == 0)
          {
            key_state[i] = 0;

            event->key.keysym.sym = i;
            break;
          }
        }
      }
      else
      {
        printf(RED_YEL "There can't be another char after 'k'\n" WRITE);
        exit(-1);
      }
      // printf("type=%d,sym=%d\n", event->type, event->key.keysym.sym);
      break;
    }
  }

  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask)
{
  printf(RED_YEL "(4)NO implement!\n" WRITE);
  exit(-1);
  return 0;
}

// Gets a snapshot of the current keyboard state.
// The current state is return as a pointer to an array, the size of this array is stored in numkeys.
// The array is indexed by the SDLK_* symbols. A value of 1 means the key is pressed and a value of 0 means its not.
uint8_t *SDL_GetKeyState(int *numkeys)
{
  // SDL_Event ev;

  if (numkeys != NULL)
  {
    *numkeys = sizeof(key_state) / sizeof(key_state[0]);
  }
  // // SDL_PumpEvents();
  return key_state;
  // printf(RED_YEL "(5)NO implement!\n" WRITE);
  // exit(-1);
  // return NULL;
}
