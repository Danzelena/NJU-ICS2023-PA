#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context *(*user_handler)(Event, Context *) = NULL;

Context *__am_irq_handle(Context *c)
{

  // 把执行流切换的原因打包成事件,然后user_handle()
  if (user_handler)
  {
    Event ev = {0};
    // printf("c->mcause:%d\n",c->mcause);
    switch (c->mcause)
    {
    case -1:
      ev.event = EVENT_YIELD;break;
    // BUG:有不干净的东西~ 这么写就会报错(
    // case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:
    case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:
      ev.event = EVENT_SYSCALL;break;

    case 0:
      ev.event = EVENT_SYSCALL;break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }
    // printf("(Debug)before schedule()\n");
    c = user_handler(ev, c);
    // printf("(Debug)after schedule()\n");
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *))
{
  // initialize exception entry
  uint64_t val = 0x1800;
  // READ: 直接将异常入口地址设置到mtvec寄存器中,__am_asm_trap is a asm function,put address of the function to reg
  asm volatile("csrw mstatus, %0"::"r"(val):);

  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

// create kernel thread
// 'k' for kernal 
// kstack -> range of stack
// entry  -> entry to kernel thread
// arg    -> parameter of kernel thread
// 此外, kcontext()要求内核线程不能从entry返回, 否则其行为是未定义的. 
// 你需要在kstack的底部创建一个以entry为入口的上下文结构(目前你可以先忽略arg参数), 然后返回这一结构的指针.
Context *kcontext(Area kstack, void (*entry)(void *), void *arg)
{
  Context *context = kstack.end - sizeof(Context) - 4;

  context->mstatus = 0x1800;
  context->mepc = (uintptr_t)entry;
  // TODO: mcause, gpr[NR_REGS], pdir
  return context;
  // return NULL;
}

void yield()
{
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled()
{
  return false;
}

void iset(bool enable)
{
}
