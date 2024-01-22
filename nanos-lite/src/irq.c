#include <common.h>
#include <proc.h>
// #include <errno.h>
void do_syscall();
static Context *do_event(Event e, Context *c)
{
  switch (e.event)
  {
  // 在 native 上进行上下文切换
  case EVENT_IRQ_TIMER:
    /* 调用schedule()强制当前进程让出CPU */
    Log("Get Timer Interrupt");
    // c->mepc -= 4;
    return schedule(c);
    break;
  case EVENT_IRQ_IODEV:
    break;
  case EVENT_YIELD:
    // 在 Nanos-lite 收到 EVENT_YIELD 之后, 使用 schedule() 来返回新的上下文
    // printf("(Debug)(EVENT_YIELD)\n");
    // printf("(yield trace)\n")
    /* Debug */
    // printf("DEBUG:Context message:\n");
    // printf("gpr:\n");
    // for (size_t i = 0; i < 32; i++)
    // {
    //   printf("%x\n", (unsigned long)c->gpr[i]);
    // }
    // printf("mcause;%x\n", c->mcause);
    // printf("mstatus;%x\n", c->mstatus);
    
    Context *ret = schedule(c);
    // printf("(Debug)(EVENT_YIELD)finish\n");
    // printf("(Debug)mepc=%x\n", ret->mepc);
    return ret;
    break;
  case EVENT_SYSCALL:

    /* strace */
    printf("(strace)Type:%d, GPR2=%x, GPR3=%x\n",c->GPR1, c->GPR2, c->GPR3);
    do_syscall(c);
    printf("(strace)finish\n");
    break;
  default:
    panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void)
{
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
