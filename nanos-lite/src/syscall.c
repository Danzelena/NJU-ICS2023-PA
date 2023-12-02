#include <common.h>
#include "syscall.h"


// This is a user_handler func for AM, will handle different syscall
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  // a[1] = c->GPR2;
  // a[2] = c->GPR3;
  // a[3] = c->GPR4;
  // printf("a:(%d,%d,%d)\n",a[1],a[2],a[3]);

  //TODO: do something depend on a[0]
  switch (a[0]) {
    case SYS_yield:
      yield();
      // int val = 0;
      // asm volatile("li (GPRx), %0");
      c->GPRx = 0;
      
      break;

    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
