#include <proc.h>

#define MAX_NR_PROC 4
#define MAX_argc 10
#define MAX_envc 10
// Bytes
#define MAX_args_len 0x400

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
  // printf("(Debug)epc1=%x\n", pcb->cp->mepc);
}

inline int argc_get(char *const argv[]){
  int count = 0;
  while(argv[count]!= NULL){
    // printf("%s\n", argv[count]);
    count ++;
  }
  return count;
}
inline int envc_get(char *const envp[]){
  int count = 0;
  while(envp[count]!= NULL){
    // printf("%s\n", envp[count]);
    count ++;
  }
  return count;
}
static size_t len_resize(size_t size){
  if(size & 0b11){
    return (size & 0b11) + 0x4;
  }
  else{
    return size;
  }
}
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  uintptr_t entry = entry_get(pcb, filename);
  assert((void*)entry!= NULL);
  uintptr_t kstack_begin = (uintptr_t)pcb->stack;
  uintptr_t kstack_end = (uintptr_t)(pcb + 1);
  assert((void*)kstack_begin!= NULL && (void*)kstack_end!= NULL&&kstack_end>kstack_begin);
  printf("(Debug)(context_uload)begin=%x, end=%x\n", pcb->stack, kstack_end);
  AddrSpace addrs;
  // TODO: context_uload
  // 分配上下文
  Context *context = ucontext(&addrs, (Area) {pcb->stack, (void*)kstack_end}, (void*)entry);

  pcb->cp = context;

  // Warning: 强行约定了 arg 区域的大小

  // uintptr_t arg_begin = pcb->cp->GPRx;
  uintptr_t arg_end = kstack_begin;
  uintptr_t arg_begin = arg_end - MAX_args_len;

  printf("(Debug)arg_begin=%x, arg_end=%x\n", arg_begin, arg_end);
  assert((void*)arg_begin!= NULL&&(void*)arg_end!= NULL&&arg_begin<arg_end);

  const int argc = argc_get(argv);
  const int envc = envc_get(envp);
  assert(argc >= 0&&envc >= 0);

  char *envp_ustack[envc];
  char *argv_ustack[argc];

  char *str_ptr = (char *)(arg_end - 4);

  // copy string area
  // printf("(Debug)argc=%d\n", argc);
  // printf("(Debug)envc=%d\n", envc);
  for(int i = 0; i < envc; i++){
    str_ptr -= (len_resize(strlen(envp[i]) + 1));// consider '\0'
    envp_ustack[i] = str_ptr;
    strcpy(str_ptr, envp[i]);
  }

  for (int i = 0; i < argc; i++){
    str_ptr -= (len_resize(strlen(argv[i] + 1)));// case '\0'
    argv_ustack[i] = str_ptr;
    strcpy(str_ptr, argv[i]);
  }


  uintptr_t *ptr_ptr = (uintptr_t *)arg_begin;
  // allocate argc
  *ptr_ptr = argc;
  ptr_ptr ++;
  
  // allocate argv
  for (int i = 0; i < argc; i ++){
    *(ptr_ptr + i) = (uintptr_t)argv_ustack[i];
  }
  ptr_ptr += argc;


  *ptr_ptr = (uintptr_t)NULL;
  ptr_ptr ++;

  // allocate envp
    for(int i = 0;i < envc; i++){
    *(ptr_ptr + i) = (uintptr_t)envp_ustack[i];
  }
  ptr_ptr += envc;
  *ptr_ptr = (uintptr_t)NULL;
  ptr_ptr ++;

  assert(ptr_ptr < (uintptr_t *)str_ptr);
  printf("(Debug)(context_uload)finish with ptr_ptr=%x, str_ptr=%x\n", ptr_ptr, str_ptr);

  pcb->cp->GPRx = arg_begin;
}
void init_proc() {
  // 创建两个以hello_fun()为入口的上下文
  context_kload(&pcb[0], hello_fun, (void *)1L);
  // context_uload(&pcb[0], "/bin/hello");

  // BUG: 根据目前计算 argc, envc的方法,必须这么定义 argv, envp
  char *const argv[] = {"--skip", "hello", "world", "NJU", NULL};
  char *const envp[] = {"ICS", "PA", "pa", NULL};
  // context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[1], "/bin/pal", argv, envp);
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
  

  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  // printf("(Debug)mepc1=%x, mepc2=%x \n", pcb[0].cp->mepc, pcb[1].cp->mepc);
  // if(current==&pcb[0]){printf("(Debug)(Schedule)go to 0\n");}
  // if(current==&pcb[1]){printf("(Debug)(Schedule)go to 1\n");}
  return current->cp;
}
