#include <common.h>
#include "syscall.h"


// This is a user_handler func for AM, will handle different syscall
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("a:(%d)\n",a[1]);

  switch (a[0]) {
    case SYS_write:
    //  TODO:check `fd` value
      int fd = a[1];
      char *buf = (char*)a[2];
      int len = a[3];
      printf("buf = %s,len=%d\n",buf,len);
      // stdout, stderr
      if(fd == 1 || fd == 2){
        for(int i = 0;i < len;i++){
          putch(buf[i]);
        }
      }
      else{
        putch('S');putch('o');putch('r');putch('r');putch('y');putch('t');putch('o');putch('W');putch('r');putch('\n');
        panic("Sorry,fd is not stdout or stderr\n");
      }
      c->GPRx = -1;
      break;


    case SYS_brk:
      // uintptr_t addr = a[1];
      c->GPRx = 0;
      break;
    case SYS_yield:
      yield();
      // int val = 0;
      // int val =0;
      // asm volatile("li a0, 123" : : "r"(val));
      c->GPRx = 0;
      // panic("a0:%d",c->GPRx);
      break;
    case SYS_exit:
      // printf("exit!Ohhh\n");
      // status: GPR2


  
      halt(a[1]);
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
