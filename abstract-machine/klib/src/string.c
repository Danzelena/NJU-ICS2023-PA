#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  // return 0;

  char *ptr;
  int len = 0;
  for(ptr = (char*)s;*ptr != '\0'; ptr++){
    len ++;
  }
  return len;
  // panic("Not implemented");
}

// the size of dst must be greater than the length of src
char *strcpy(char *dst, const char *src) {
  size_t i;
  for(i = 0;src[i] != '\0';i++){
    dst[i] = src[i];

  }
  dst[i] = '\0';
  return dst;

  // panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t i;
  for(i = 0; i < n && src[i] != '\0';i ++){
    dst[i] = src[i];
  }
  for(;i < n;i++){
    dst[i] = '\0';
  }
  return dst;
  // panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  size_t dst_len = strlen(dst);
  size_t i;
  for (i=0;src[i] != '\0';i++){
    dst[dst_len + i] = src[i];

  }
  dst[dst_len + i] = '\0';
  return dst;
  // panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  size_t i;
  for(i = 0;s1[i] != '\0' && s2[i] != '\0';i++){
    if(s1[i] != s2[i]){
      return (size_t)s1[i] - (size_t)s2[i];
    }
  }
  return 0;
  // panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
    size_t i;
    for(i = 0;s1[i] != '\0' && s2[i] != '\0'&& i < n;i++){
    if(s1[i] != s2[i]){
      return (size_t)s1[i] - (size_t)s2[i];
    }
   
  }
   return 0;
  // panic("Not implemented");
}

// fill a memory with a constant byte
void *memset(void *s, int c, size_t n) {
  size_t i;
  unsigned char *p = s;
  for(i = 0;i < n;i ++){
    *(p + i) = c;
  }
  return s;
  // panic("Not implemented");
}

void *memcpy();
void *memmove(void *dst, const void *src, size_t n) {
  // copy direction depends on how buffers overleap
  
  // case 1:
  // dst: dddddd
  // src:    sssssss

  if((uint32_t*)dst < (uint32_t*)src){
    return memcpy(dst,src,n);
  } 
  uint8_t* d = dst;
  const uint8_t* s = src;

  size_t i;
  for(i = n;i > 0;i--){
    d[i-1] = s[i-1];
  }
  return dst;
  // panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  size_t i;
  uint8_t *d = out;
  const uint8_t *s = in;
  for(i = 0;i < n;i++){
    d[i]= s[i];

  }
  return out;
  // panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
    size_t i;
    uint8_t *a = (uint8_t*)s1;
    uint8_t *b = (uint8_t*)s2;
    for(i = 0;i < n;i++){
      if(*(a + i) != *(b+i)){
        return (size_t)*(a + i) - (size_t)*(b + i);
    }
    
  }
  return 0;
  // panic("Not implemented");
}

#endif
