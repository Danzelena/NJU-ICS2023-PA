#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  assert(freesrc == 0);
  return NULL;
}


// 接受一个图片文件的路径,然后吧像素信息封装为 Surfaace 结构.
SDL_Surface* IMG_Load(const char *filename) {
  /*用libc中的文件操作打开文件, 并获取文件大小size*/
  printf("filename=%s\n",filename);
  FILE* fd = fopen(filename,"r");
  if(fd == NULL){
    printf("fail to open image\n");
    exit(-1);
  }
  // get size
  fpos_t pos;
  fgetpos(fd,&pos);
  fseek(fd,0,SEEK_END);
  long size = ftell(fd);
  fsetpos(fd,&pos);

  /*申请一段大小为size的内存区间buf*/
  char *buf = malloc(size);
  /*将整个文件读取到buf中*/
  int flag = fread(buf,1,size,fd);
  if(flag != size){
    printf("flag != size\n");
    exit(-1);
  }
  /*将buf和size作为参数, 调用STBIMG_LoadFromMemory(), 它会返回一个SDL_Surface结构的指针*/
  SDL_Surface *s = STBIMG_LoadFromMemory(buf,size);
  /*关闭文件, 释放申请的内存*/
  fclose(fd);
  free(buf);
  /*返回SDL_Surface结构指针*/
  return s;
  // return NULL;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
