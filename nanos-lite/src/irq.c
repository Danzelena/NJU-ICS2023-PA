#include <common.h>
#include <proc.h>

void do_syscall();
static Context *do_event(Event e, Context *c)
{
  switch (e.event)
  {
  // 在 native 上进行上下文切换
  case EVENT_IRQ_TIMER:
    break;
  case EVENT_YIELD:
    // 在 Nanos-lite 收到 EVENT_YIELD 之后, 使用 schedule() 来返回新的上下文
    printf("(Debug)(EVENT_YIELD)\n");
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
    // printf("(Debug)mepc=%x\n", ret->mepc);
    return ret;
    break;
  case EVENT_SYSCALL:

    /* strace */
    // printf("(strace)Type:%d\n",c->GPR1);
    do_syscall(c);

    // panic("Syscall ID = %d", e.event);
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
