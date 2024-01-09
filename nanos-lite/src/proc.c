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
    yield();
  }
}

void naive_uload(PCB *pcb, const char *filename);

// 调用 kcontext 来创建上下文
// 把返回的指针记录到 PCB 的 cp 中
void context_kload(PCB *pcb, void (*entry)(void *), void *arg){
  printf("(Debug)(context_kload)begin=%x, end=%x\n", pcb->stack, pcb + 1);
  pcb->cp = kcontext((Area) {pcb->stack, pcb + 1}, entry, arg);
}

void init_proc() {
  // 创建两个以hello_fun()为入口的上下文
  context_kload(&pcb[0], hello_fun, (void *)1L);
  context_kload(&pcb[1], hello_fun, (void *)2L);
  switch_boot_pcb();

  Log("Initializing processes...");
  yield();
  panic("Show not reach here\n");
  // load program here
  char *filename = "/bin/menu";
  naive_uload(NULL,filename);
}

Context* schedule(Context *prev) {
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  return current->cp;
}
