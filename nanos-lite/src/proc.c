#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

// 初始化 current 指针
void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    for (int volatile i = 0; i < 100000; i++) ;// make it slower
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);
uintptr_t entry_get(PCB *pcb, const char *filename);

// 调用 kcontext 来创建上下文
// 把返回的指针记录到 PCB 的 cp 中
void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  printf("(Debug)(context_kload)begin=%x, end=%x\n", pcb->stack, pcb + 1);
  pcb->cp = kcontext((Area) {pcb->stack, pcb + 1}, entry, arg);
  printf("(Debug)epc1=%x\n", pcb->cp->mepc);
}
void context_uload(PCB *pcb, const char *filename){
  uintptr_t entry = entry_get(pcb, filename);
  uintptr_t kstack_end = (uintptr_t)pcb + 1;
  printf("(Debug)(context_kload)begin=%x, end=%x\n", pcb->stack, pcb + 1);
  Context *context = ucontext(NULL, (Area) {pcb->stack, (void*)kstack_end}, (void*)entry);
  context->GPRx = kstack_end;
  pcb->cp = context;
  // printf("(Debug)pcb->cp->mepc=%x\n", pcb->cp->mepc);
}
void init_proc() {
  // 创建两个以hello_fun()为入口的上下文
  context_kload(&pcb[0], hello_fun, (void *)1L);
  context_uload(&pcb[1], "/bin/pal");
  // context_kload(&pcb[1], hello_fun, (void *)2L);
  switch_boot_pcb();

  Log("Initializing processes...");
  yield();
  panic("Show not reach here\n");
  // load program here
  char *filename = "/bin/menu";
  naive_uload(NULL,filename);
}

Context* schedule(Context *prev) {
  // printf("(Debug)begin scuedule\n");
  current->cp = prev;
  // if(current==&pcb[0]){printf("1\n");}else {printf("0\n");}
  // if(current==&pcb[1]){printf("1\n");}else {printf("0\n");}

  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  printf("(Debug)mepc1=%x, mepc2=%x\n", pcb[0].cp->mepc, pcb[1].cp->mepc);

  return current->cp;
}
