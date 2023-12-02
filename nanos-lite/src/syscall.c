#include <common.h>
#include "syscall.h"


// This is a user_handler func for AM, will handle different syscall
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  // a[2] = c->GPR3;
  // a[3] = c->GPR4;
  // printf("a:(%d)\n",a[1]);

  //TODO: do something depend on a[0]
  switch (a[0]) {
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
