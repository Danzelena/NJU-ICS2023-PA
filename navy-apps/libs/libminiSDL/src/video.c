
#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RED_YEL "\033[31;43m"
#define WRITE "\033[0m"

uint32_t gen_pixel(SDL_Surface *surface, int i, int j);

// Warning:
void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
  
  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);

  if (src->format->BitsPerPixel == 32){
    uint32_t* src_pixels = (uint32_t*)src->pixels;
    uint32_t* dst_pixels = (uint32_t*)dst->pixels;

    int rect_w, rect_h, src_x, src_y, dst_x, dst_y;
    if (srcrect){
      rect_w = srcrect->w; rect_h = srcrect->h;
      src_x = srcrect->x; src_y = srcrect->y; 
    }else {
      rect_w = src->w; rect_h = src->h;
      src_x = 0; src_y = 0;
    }
    if (dstrect){
      dst_x = dstrect->x, dst_y = dstrect->y;
    }else {
      dst_x = 0; dst_y = 0;
    }
    
    for (int i = 0; i < rect_h; ++i){
      for (int j = 0; j < rect_w; ++j){
        dst_pixels[(dst_y + i) * dst->w + dst_x + j] = src_pixels[(src_y + i) * src->w + src_x + j];
      }
    }
  }else if (src->format->BitsPerPixel == 8){
    uint8_t* src_pixels = (uint8_t*)src->pixels;
    uint8_t* dst_pixels = (uint8_t*)dst->pixels;

    int rect_w, rect_h, src_x, src_y, dst_x, dst_y;
    if (srcrect){
      rect_w = srcrect->w; rect_h = srcrect->h;
      src_x = srcrect->x; src_y = srcrect->y; 
    }else {
      rect_w = src->w; rect_h = src->h;
      src_x = 0; src_y = 0;
    }
    if (dstrect){
      dst_x = dstrect->x, dst_y = dstrect->y;
    }else {
      dst_x = 0; dst_y = 0;
    }
    
    for (int i = 0; i < rect_h; ++i){
      for (int j = 0; j < rect_w; ++j){
        dst_pixels[(dst_y + i) * dst->w + dst_x + j] = src_pixels[(src_y + i) * src->w + src_x + j];
      }
    }
  }else {
    assert(0);
  }
  
} 


// Warning:
void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color)
{ 
  uint32_t *pixels = (uint32_t *)dst->pixels;
  int dst_w = dst->w;
  int rect_h, rect_w, rect_x, rect_y;

  if (dstrect == NULL){
    rect_w = dst->w;
    rect_h = dst->h;
    rect_x = 0;
    rect_y = 0;
  }else {
    rect_w = dstrect->w;
    rect_h = dstrect->h;
    rect_x = dstrect->x;
    rect_y = dstrect->y;
  }

  for (int i = 0; i < rect_h; ++i){
    for (int j = 0; j < rect_w; ++j){
      pixels[(rect_y + i) * dst_w + rect_x + j] = color;
    }
  }
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h)
{
  // printf("SDL_UpdateRect\n");

  if (x == 0 && y == 0 && w == 0 && h == 0)
  {
    /* update the entire screen */
    w = s->w;
    h = s->h;
  }

  uint32_t *pixels = malloc(w * h * sizeof(uint32_t));
  assert(pixels);
  // printf("1:pixels=%x\n",pixels);
  // printf("x=%d,y=%d,w=%d,h=%d,s->w=%d,s->h=%d\n", x, y, w, h,s->w,s->h);
  // printf("Target1\n");
  for (int j = y; j < y + h; j++)
  {
    for (int i = x; i < x + w; i++)
    {
      uint32_t pix = gen_pixel(s, i, j);
      pixels[(j-y) * w + (i-x)] = pix;// pixels=w*h
      // printf("pixels=%x\n",pix);
    }
  }
  // printf("2:pixels=%x\n",pixels);
  // printf("Target2 in vidio.c\n");
  // printf("3:pixels=%x\n",pixels);

  NDL_DrawRect(pixels, x, y, w, h);
  // fixedBUG:double free or corruption(!prev)
  // Aborted(core dumped)
  // if(pixels == NULL){
  //   printf("pixel is NULL\n");
  // }
    // printf("4:pixels=%x\n",pixels);

  free(pixels);
  // printf("Target3 in video.c\n");
  // printf("SDL_UpdateRect finish\n");
}

// return pixel(AGRB) of surface on (i,j)
// ------------>i(x)
// |
// j(y)
uint32_t gen_pixel(SDL_Surface *surface, int i, int j)
{
  SDL_PixelFormat *fmt = surface->format;
  if (fmt->BytesPerPixel == 1)
  {
    assert(fmt->BytesPerPixel == 1);
    uint8_t index;
    SDL_Color *color;
    /* Check the bitdepth of the surface */
    if (fmt->BitsPerPixel != 8)
    {
      printf(RED_YEL "Not an 8-bit surface.\n" WRITE);
      exit(-1);
      // return (-1);
    }

    /* Lock the surface(NO implement) */
    // SDL_LockSurface(surface);

    /* Get the topleft pixel */
    index = *((uint8_t *)surface->pixels + j * surface->w + i);
    color = fmt->palette->colors + index;

    /* Unlock the surface(NO implement) */
    // SDL_UnlockSurface(surface);

    // printf("Pixel Color-> Red: %d, Green: %d, Blue: %d. Index: %d\n",color->r, color->g, color->b, index);
    // printf("Debug:Rshift=%d\nGshift=%d\nBshift=%d\nAshift=%d\n",fmt->Rshift,fmt->Gshift,fmt->Bshift,fmt->Ashift);

    uint32_t p = (color->r << 16) | (color->g << 8) | (color->b << 0);
    if (fmt->Amask)
      p |= (color->a << 24);
    return p;
  }
  else
  {
    uint32_t temp, pixel;
    uint32_t red, green, blue, alpha;

    fmt = surface->format;
    // SDL_LockSurface(surface);
    pixel = *((uint32_t *)surface->pixels + j * surface->w + i);
    // SDL_UnlockSurface(surface);

    /* Get Red component */
    temp = pixel & fmt->Rmask;  /* Isolate red component */
    temp = temp >> fmt->Rshift; /* Shift it down to 8-bit */
    temp = temp << fmt->Rloss;  /* Expand to a full 8-bit number */
    red = (uint8_t)temp;

    /* Get Green component */
    temp = pixel & fmt->Gmask;  /* Isolate green component */
    temp = temp >> fmt->Gshift; /* Shift it down to 8-bit */
    temp = temp << fmt->Gloss;  /* Expand to a full 8-bit number */
    green = (uint8_t)temp;

    /* Get Blue component */
    temp = pixel & fmt->Bmask;  /* Isolate blue component */
    temp = temp >> fmt->Bshift; /* Shift it down to 8-bit */
    temp = temp << fmt->Bloss;  /* Expand to a full 8-bit number */
    blue = (uint8_t)temp;

    /* Get Alpha component */
    temp = pixel & fmt->Amask;  /* Isolate alpha component */
    temp = temp >> fmt->Ashift; /* Shift it down to 8-bit */
    temp = temp << fmt->Aloss;  /* Expand to a full 8-bit number */
    alpha = (uint8_t)temp;
    // uint32_t p = red | green | blue;
    // printf("r=%x,g=%x,b=%x\n",red,green,blue);
    uint32_t p = (red << 16) | (green << 8) | (blue << 0);
    
    if (fmt->Amask)
      p |= (alpha << 24);
    // printf("p=%x\n",p);
    return p;
    // printf("Pixel Color -> R: %d,  G: %d,  B: %d,  A: %d\n", red, green, blue, alpha);
  }
}
// APIs below are already implemented.

static inline int maskToShift(uint32_t mask)
{
  switch (mask)
  {
  case 0x000000ff:
    return 0;
  case 0x0000ff00:
    return 8;
  case 0x00ff0000:
    return 16;
  case 0xff000000:
    return 24;
  case 0x00000000:
    return 24; // hack
  default:
    assert(0);
  }
}

SDL_Surface *SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
                                  uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8)
  {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  }
  else
  {
    s->format->palette = NULL;
    s->format->Rmask = Rmask;
    s->format->Rshift = maskToShift(Rmask);
    s->format->Rloss = 0;
    s->format->Gmask = Gmask;
    s->format->Gshift = maskToShift(Gmask);
    s->format->Gloss = 0;
    s->format->Bmask = Bmask;
    s->format->Bshift = maskToShift(Bmask);
    s->format->Bloss = 0;
    s->format->Amask = Amask;
    s->format->Ashift = maskToShift(Amask);
    s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC))
  {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface *SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
                                      int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
                                        Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s)
{
  if (s != NULL)
  {
    if (s->format != NULL)
    {
      if (s->format->palette != NULL)
      {
        if (s->format->palette->colors != NULL)
          free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC))
      free(s->pixels);
    free(s);
  }
}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags)
{
  if (flags & SDL_HWSURFACE)
    NDL_OpenCanvas(&width, &height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
                              DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  if (w == dstrect->w && h == dstrect->h)
  {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else
  {
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors)
{
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if (s->flags & SDL_HWSURFACE)
  {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i++)
    {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len)
{
  int i;
  uint8_t(*pdst)[4] = dst;
  uint8_t(*psrc)[4] = src;
  union
  {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16)
  {
#define macro(i)                      \
  tmp.val32 = *((uint32_t *)psrc[i]); \
  *((uint32_t *)pdst[i]) = tmp.val32; \
  pdst[i][0] = tmp.val8[2];           \
  pdst[i][2] = tmp.val8[0];

    macro(i + 0);
    macro(i + 1);
    macro(i + 2);
    macro(i + 3);
    macro(i + 4);
    macro(i + 5);
    macro(i + 6);
    macro(i + 7);
    macro(i + 8);
    macro(i + 9);
    macro(i + 10);
    macro(i + 11);
    macro(i + 12);
    macro(i + 13);
    macro(i + 14);
    macro(i + 15);
  }

  for (; i < len; i++)
  {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags)
{
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface *ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
                                          fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

// READ:convert <r,g,b,a> to uint32_t
uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask)
    p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s)
{
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s)
{
}
