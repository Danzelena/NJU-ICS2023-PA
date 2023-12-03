#include <common.h>


void do_syscall();
static Context *do_event(Event e, Context *c)
{
  switch (e.event)
  {
  case EVENT_YIELD:
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
    // printf("mepc;%x\n", c->mepc);
    
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
