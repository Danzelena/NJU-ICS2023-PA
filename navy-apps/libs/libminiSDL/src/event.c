
#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,
#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

static const char *keyname[] = {
    "NONE",
    _KEYS(keyname)};

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
    printf(RED_YEL "(SDL_WaitEvent)buf is not signle line!\n" WRITE);
    exit(-1);
  }
  char dir;
  char key[16];
  sscanf(buf, "k%c %s", &dir, key);

  if (dir == 'd')
  {
    ev->type = SDL_KEYDOWN;
  }
  else if (dir == 'u')
  {
    ev->type = SDL_KEYUP;
  }
  else
  {
    printf(RED_YEL "(SDL_WaitEvent)no another case!\n" WRITE);
    exit(-1);
  }

  for (int i = 0; i < sizeof(keyname) / sizeof(char); i++)
  {
    // printf("key=%s,keyname[i]=%s\n",key,keyname[i]);
    if (strcmp(key, keyname[i]) == 0)
    {
      ev->key.keysym.sym = i;
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
        printf("down!\n");
        printf("KEY=%s\n", key);

        if (strcmp(key, "DOWN") == 0)
        {
          event->key.keysym.sym = SDLK_DOWN;
        }
        else if (strcmp(key, "UP") == 0)
        {
          event->key.keysym.sym = SDLK_UP;
          printf("SDLK_UP\n");
        }
        else if (strcmp(key, "RIGHT") == 0)
        {
          event->key.keysym.sym = SDLK_RIGHT;
          printf("SDLK_RIGHT\n");
        }
        else if (strcmp(key, "LEFT") == 0)
        {
          event->key.keysym.sym = SDLK_LEFT;
          printf("SDLK_LEFT\n");
        }
        else
        {
          // BUG: don't know how to set status
          event->key.keysym.sym = SDLK_NONE;
        }
      }
      else if (dir == 'u')
      {
        event->type = SDL_KEYUP;
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

uint8_t *SDL_GetKeyState(int *numkeys)
{
  printf(RED_YEL "(5)NO implement!\n" WRITE);
  exit(-1);
  return NULL;
}
