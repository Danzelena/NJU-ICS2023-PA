#include <SDL_bmp.h>
#include <BMP.h>
#include <assert.h>
#include <stdio.h>

SDL_Surface* SDL_LoadBMP(const char *filename) {
  int w, h;
  
  void *pixels = BMP_Load(filename, &w, &h);
  if(pixels == NULL){
    printf("3.1is Null\n");
  }
  printf("filename:%s\n",filename);
  if(pixels == NULL){
    printf("3is Null\n");
  }
  assert(pixels);// where BUG exists
  printf("pass assert33!\n");
  SDL_Surface *s = SDL_CreateRGBSurfaceFrom(pixels, w, h, 32, w * sizeof(uint32_t),
      DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
  printf("pass assert!\n");
  s->flags &= ~SDL_PREALLOC;
  return s;
}
