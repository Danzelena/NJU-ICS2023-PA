#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

#define IRQ_TIMER 0x80000007
static Context *(*user_handler)(Event, Context *) = NULL;
void __am_get_cur_as(Context *c);
void __am_switch(Context *c);

#define KERNEL 3
#define USER 0

Context *__am_irq_handle(Context *c)
{
  uintptr_t mscratch_r;
  uintptr_t kas = 0;
  asm volatile("csrr %0, mscratch" : "=r"(mscratch_r));

  c->np = (mscratch_r == 0 ? KERNEL : USER);
  printf("(Debug)c->np=%x", c->np);
  asm volatile("csrw mscratch, %0" : : "r"(kas));

  // printf("(Debug)buf0=%x\n", c->GPR3);
  __am_get_cur_as(c);
  printf("(__am_irq_handle)pdir=%x\n", c->pdir);
  // printf("(__am_irq_handle)2\n");
  // 把执行流切换的原因打包成事件,然后user_handle()
  if (user_handler)
  {
    Event ev = {0};
    // printf("c->mcause:%d\n",c->mcause);
    switch (c->mcause)
    {
    case -1:
      ev.event = EVENT_YIELD;break;
 
    // case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:
    case 1:case 2:case 3:case 4:case 5:case 6:case 7:case 8:case 9:case 10:case 11:case 12:case 13:case 14:case 15:case 16:case 17:case 18:case 19:
      ev.event = EVENT_SYSCALL;break;
    case 0:
      ev.event = EVENT_SYSCALL;break;
    case IRQ_TIMER:
      ev.event = EVENT_IRQ_TIMER;
      c->mepc -= 4;
      break;
    default:
      ev.event = EVENT_ERROR;
      break;
    }
    // printf("(Debug)ev.event=%d, buf=%x\n", ev.event, c->GPR3);
    c = user_handler(ev, c);
    // printf("(Debug)after schedule()\n");
    assert(c != NULL);
  }
  // printf("(__am_ieq_handle)3\n");
  // assert(c->pdir == )
  __am_switch(c);
  // printf("(__am_ieq_handle)4\n");
  printf("(DEBUG)c->sp=%x\n", c->GPRsp);
  printf("===========================================\n");

  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context *(*handler)(Event, Context *))
{
  // ASSERT:
  // assert(0);
  // initialize exception entry
  uint64_t val = 0x1800;
  // READ: 直接将异常入口地址设置到mtvec寄存器中,__am_asm_trap is a asm function,put address of the function to reg
  asm volatile("csrw mstatus, %0"::"r"(val):);

  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;
  // ASSERT: here
  // assert(0);
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
  // ASSERT here
  // assert(0);

  uintptr_t *buf = kstack.end - 4;
  *buf = 0;

  Context *context = kstack.end - sizeof(Context) - 4;
  //TODO: for test
  context->mstatus = 0x1800 | 0x80;
  context->mepc = (uintptr_t)entry;
  // context->m
  // riscv 是使用寄存器传递参数
  context->GPRa0 = (uintptr_t)arg;
  context->pdir = NULL;

  context->np = KERNEL;
  context->GPRsp = (uintptr_t)kstack.end - 4;
  // ASSERT: here
  // assert(0);
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
