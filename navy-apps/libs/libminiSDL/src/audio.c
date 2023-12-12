#include <NDL.h>
#include <SDL.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

// Opens the audio device with the desired parameters.
int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained) {
  printf(RED_YEL "(OpenAudio)NO implement!\n" WRITE);
  // exit(-1);
  return 0;
}

void SDL_CloseAudio() {
    printf(RED_YEL "(CloseAudio)NO implement!\n" WRITE);
  exit(-1);
}

void SDL_PauseAudio(int pause_on) {
    printf(RED_YEL "(PauseAudio)NO implement!\n" WRITE);
  // exit(-1);
}

void SDL_MixAudio(uint8_t *dst, uint8_t *src, uint32_t len, int volume) {
    printf(RED_YEL "(MixAudio)NO implement!\n" WRITE);
  exit(-1);
}

SDL_AudioSpec *SDL_LoadWAV(const char *file, SDL_AudioSpec *spec, uint8_t **audio_buf, uint32_t *audio_len) {
  printf(RED_YEL "(SDL_LoadWAV)NO implement!\n" WRITE);
  // exit(-1);
  return NULL;
}

void SDL_FreeWAV(uint8_t *audio_buf) {
    printf(RED_YEL "(FreeWAV)NO implement!\n" WRITE);
  exit(-1);
}

void SDL_LockAudio() {
    printf(RED_YEL "(LockAudio)NO implement!\n" WRITE);
  // exit(-1);
}

void SDL_UnlockAudio() {
    printf(RED_YEL "(UnlockAudio)NO implement!\n" WRITE);
  // exit(-1);
}
