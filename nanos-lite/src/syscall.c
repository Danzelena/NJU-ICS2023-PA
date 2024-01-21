#include <common.h>
#include "syscall.h"
#include "../include/fs.h"
#include <sys/time.h>
#include <proc.h>

int mm_brk(uintptr_t brk);
void naive_uload();
void switch_boot_pcb();
void context_uload();

// This is a user_handler func for AM, will handle different syscall
void do_syscall(Context *c)
{
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // printf("a:(%d)\n",a[1]);

  switch (a[0])
  {
  case SYS_lseek:
    c->GPRx = fs_lseek(a[1],a[2],a[3]);
    break;
  case SYS_open:
    c->GPRx = fs_open((char*)a[1],a[2],a[3]);
    break;
  case SYS_close:
    c->GPRx = fs_close(a[1]);
    
    // return 0;
    break;
  case SYS_read:
    c->GPRx = fs_read(a[1],(void*)a[2],a[3]);
    break;
  case SYS_execve:
    printf("(Debug)(SYS_execve)\n");
    const char *path = (char *)a[1];
    char **const argv = (char **)a[2];
    char **const envp = (char **)a[3];
  //     printf("(Debug)argv=%s\n", argv);
  // printf("(Debug)envp=%s\n", envp);
    context_uload(current, path, argv, envp);
    switch_boot_pcb();
    yield();
    panic("Should not reach here\n");
    naive_uload(NULL,(char *)a[1]);
    c->GPRx = -1;
    break;
  case SYS_write:
    printf("(SYS_write)a[1]=%x, a[2]=%x, a[3]=%x\n", a[1], a[2], a[3]);
    c->GPRx = fs_write(a[1],(void*)a[2],a[3]);
    break;
  case SYS_gettimeofday:
    //TODO:add system action
    struct timeval *tv = (struct timeval *)a[1];
    // struct timezone *tz = (struct timezone *)a[2];
    if(tv == NULL){
      panic("tv is NULL\n");
    }
    AM_TIMER_UPTIME_T usr;
    usr = io_read(AM_TIMER_UPTIME);
    tv->tv_sec = (usr.us/1000000);
    tv->tv_usec = (usr.us%1000000);
    c->GPRx = 0;
    break;
  case SYS_brk:
    // uintptr_t addr = a[1];
    c->GPRx = mm_brk(a[1]);
    // c->GPRx = 0;
    break;
  case SYS_yield:
    printf("(SYS_yield)\n");
    yield();
    // int val = 0;
    // int val =0;
    // asm volatile("li a0, 123" : : "r"(val));
    c->GPRx = 0;
    // panic("a0:%d",c->GPRx);
    break;
  case SYS_exit:
    // printf("exit!\n");
    // Warning:在exit后默认退回nterm
    // naive_uload(NULL,"/bin/nterm");

    printf("(SYS_exit)0x%x, 0x%x\n", a[1], a[2]);
    // status: GPR2

    halt(a[1]);
    break;

  default:
    panic("Unhandled syscall ID = %d", a[0]);
  }
}
