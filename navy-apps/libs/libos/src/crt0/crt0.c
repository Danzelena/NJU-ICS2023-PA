#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define MAX_argc 10
#define MAX_envc 10
int main(int argc, char *argv[], char *envp[]);
extern char **environ;

// TODO:解析出 argc, argv, envp, 并且调用 main
// args -> argc 的地址
void call_main(uintptr_t *args) {
  printf("(Debug)(call main)begin\n");
  assert(args!= NULL);
  uintptr_t *ptr = args;
  int argc = (int)*ptr;
  ptr ++;

  printf("(Debug)ptr=%x, args=%x\n", ptr, args);

  char *argv[MAX_argc];
  assert(MAX_argc > argc);
  char *envp[MAX_envc];
  

  size_t i = 0;
  while(*ptr != NULL){
    argv[i] = (char*)*(ptr);
    ptr ++;
    i ++;
  }
  ptr ++;// now pointer to envp
  assert(i == argc);
  
  
  i = 0;
  while(*ptr != NULL){
    envp[i] = (char*)*(ptr);
    ptr ++;
    i ++;
  }
  size_t envc = i;
  assert(MAX_envc > envc);
  // char *evnp[] = {NULL};

  // char *empty[] =  {NULL };
  // environ = empty;
  environ = envp;
  printf("(Debug)before main: argc=%d\n", argc);
  for(size_t i = 0;i < argc;i ++){
    printf("(Debug)argv[%d]=%s\n", i, argv[i]);
  }
  printf("(Debug)(call_main)finished\n");
  exit(main(argc, argv, envp));
  // exit(main(0, empty, empty));
  assert(0);
}
