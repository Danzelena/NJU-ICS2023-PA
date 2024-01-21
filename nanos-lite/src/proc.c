#include <proc.h>

#define MAX_NR_PROC 4
#define MAX_argc 10
#define MAX_envc 10
// Bytes
#define MAX_args_len 0x400
#define NR_PAGE 8
#define HELLO_RATE 1

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
    // BUG: 这一行Log必须要在下面添加一些语句,才不会影响到pcb[1].max_brk的值
    Log("Hello World from Nanos-lite with arg '%x' for the %dth time!", (uintptr_t)arg, j);
    // Log("Hello world");
    // printf("\n");
    j ++;
    printf("max_brk[0]=%x, [1]=%x\n", pcb[0].max_brk, pcb[1].max_brk);
    for (int volatile i = 0; i < HELLO_RATE; i++) ;// make it slower
    // assert(0);
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
  printf("(Debug)(context_kload)end\n");

  // pcb->max_brk = ROUN
  // printf("(Debug)epc1=%x\n", pcb->cp->mepc);
}

int argc_get(char *const argv[]){
  if(argv == NULL)return 0;
  assert(argv != NULL);
  int count = 0;
  while(argv[count]!= NULL){
    // printf("(Debug)(argc_get)%s\n", argv[count]);
    count ++;
  }
  return count;
}
inline int envc_get(char *const envp[]){
  if(envp == NULL)return 0;
  assert(envp);
  // if(envp == NULL){
  //   return 0;
  // }
  // assert(envp[0]);
  // printf("(Debug)(envc_get)%s\n", envp[0]);
  int count = 0;
  while(envp[count]){
    // printf("(Debug)(envc_get)%x\n", envp[count]);
    // printf("count=%d\n", count);
    count ++;
  }
  
  return count;
}
static size_t len_resize(size_t size){
  if(size & 0b11){
    return (size & (~0b11)) + 0x4;
  }
  else{
    return size;
  }
}
uintptr_t entry;
// TODO: 虚拟化
void context_uload(PCB *pcb, const char *filename, char *const argv[], char *const envp[]){
  printf("(Debug)(context_uload)");
  /* 在加载用户程序之前, 创建地址空间 */
  /* 调用protext()创建地址空间,需要创建内核映射(参考vme.c) */
  AddrSpace *pcb_as = &pcb->as;
  protect(pcb_as);

  /* 修改loader()函数, 支持虚拟内存加载 */

 
  printf("(Debug)(context_uload)\n");
  printf("filename=%s\n", filename);

  // Warning: 获取 argc, envc需要靠前
  const int argc = argc_get(argv);
  const int envc = envc_get(envp);

  printf("(Debug)argc = %d, envc = %d\n", argc, envc);

  // Warning: 强行约定了 arg 区域的大小, 以及让用户栈的结尾成为heap.end
  // 使用new_page()开辟新的用户栈, arg_end即用户栈栈顶
  uintptr_t arg_begin = (uintptr_t)new_page(NR_PAGE) + NR_PAGE * PGSIZE - MAX_args_len;
  uintptr_t arg_end = arg_begin + MAX_args_len;
  uintptr_t ustack_end = arg_end;
  // uintptr_t ustack_begin;

  printf("(Debug)arg_begin=%x, arg_end=%x\n", arg_begin, arg_end);
  printf("(Log)user stack\n [0x---, 0x%x]\n", ustack_end);

  assert((void*)arg_begin!= NULL&&(void*)arg_end!= NULL&&arg_begin<arg_end);

  assert(argc >= 0&&envc >= 0);
  /* 用户栈虚拟化 */
  size_t i = 8;
  assert(pcb_as->area.end - i * PGSIZE >=0);
  assert(ustack_end - i * PGSIZE>= 0);
  for(;i > 0;i--){
    map(pcb_as, (void *)(pcb_as->area.end - i * PGSIZE), (void *)(ustack_end - i * PGSIZE), 1);
  }


  char *envp_ustack[envc];
  char *argv_ustack[argc];

  char *str_ptr = (char *)(arg_end - 4);

  for(int i = 0; i < envc; i++){
    size_t resize_len = len_resize(strlen(envp[i]) + 1);
    
    str_ptr -= resize_len;// consider '\0'
    assert(resize_len >= strlen(envp[i]) + 1);
    envp_ustack[i] = str_ptr;
    
    strcpy(str_ptr, envp[i]);

    // printf("(Debug)envp[%d]=%s, re_len=%d\n", i, envp[i], resize_len);
    // printf("(Debug)(Check)envp[%d]=%s\n", i, str_ptr);
  }

  for (int i = 0; i < argc; i++){
    size_t resize_len = len_resize(strlen(argv[i]) + 1);
    str_ptr -= (len_resize(strlen(argv[i]) + 1));// case '\0'
    argv_ustack[i] = str_ptr;
    assert(resize_len >= strlen(argv[i]) + 1);
    strcpy(str_ptr, argv[i]);


    // printf("(Debug)argv[%d]=%s, re_len=%d\n", i, argv[i], resize_len);
    // printf("(Debug)(Check)argv[%d]=%s\n", i, str_ptr);
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
  // printf("(Debug)(context_uload)finish with ptr_ptr=%x, str_ptr=%x\n", ptr_ptr, str_ptr);

  entry = entry_get(pcb, filename);
  assert((void*)entry!= NULL);

  // Warning: 这条操作会把参数的内存空间扬了，要放在最后
  // 在PCB的内核栈中创建用户进程B的上下文
  uintptr_t kstack_begin = (uintptr_t)pcb->stack;
  uintptr_t kstack_end = (uintptr_t)(pcb + 1);
  assert((void*)kstack_begin!= NULL && (void*)kstack_end!= NULL&&kstack_end>kstack_begin);
  // printf("(Debug)(context_uload)begin=%x, end=%x\n", pcb->stack, kstack_end);
  // AddrSpace addrs;
  // Hint: c->uc.uc_mcontext.gregs[REG_RIP]存放 entry
  // 分配上下文
  // printf("(Debug)pcb_as->pdir=%x\n",pcb_as->ptr);
  Context *context = ucontext(pcb_as, (Area) {(void *)kstack_begin, (void*)kstack_end}, (void*)entry);
  context->GPRx = arg_begin;

  // context-> pdir = pcb_as->ptr;
  pcb->cp = context;
  // pcb->cp->GPRx = arg_begin;
  // pcb->cp->pdir = pcb_as->ptr;
}
void init_proc() {
  // naive_uload(&pcb[0],"/bin/dummy");
  context_kload(&pcb[0], hello_fun, (void *)1L);
  // context_uload(&pcb[0], "/bin/dummy", NULL, NULL);

  // BUG: 根据目前计算 argc, envc的方法,必须这么定义 argv, envp
  // char *const argv[] = {"--skip", "hello", "world", "NJU", NULL};
  // char *const envp[] = {"ICS", "PA", "pa", NULL};

  // BUG: "nil" for NULL, always make Segemetion fault when getenv()
  // char *const argv[] = {"/bin/exec-test", NULL};
  // char *const envp[] = {"nil", NULL};
  
  // context_uload(&pcb[1], "/bin/pal", argv, envp);
  // context_uload(&pcb[1], "/bin/menu", argv, envp);
  context_uload(&pcb[1], "/bin/pal", NULL, NULL);


  printf("(DEBUG)(init_proc)pcb[0]->pdir=%x, mstatus=%x\n", pcb[0].cp->pdir, pcb[0].cp->mstatus);
  printf("(DEBUG)(init_proc)pcb[1]->pdir=%x, mstatus=%x\n", pcb[1].cp->pdir, pcb[1].cp->mstatus);
  // context_kload(&pcb[1], hello_fun, (void *)2L);
  switch_boot_pcb();
  Log("Initializing processes...");
  
  yield();
  panic("Show not reach here1\n");
  // load program here
  // char *filename = "/bin/menu";
  // naive_uload(NULL,filename);
}

Context* schedule(Context *prev) {
  printf("(DEBUG)prev->pdir=%x\n", prev->pdir);
  current->cp = prev;
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  // printf("(Debug)(Schedule)finished\n");
  printf("(DEBUG)pcb[0]->as->pdir=%x\n", pcb[0].as.ptr);
  printf("(DEBUG)pcb[1]->as->pdir=%x\n", pcb[1].as.ptr);
  printf("(DEBUG)pcb[0]->pdir=%x, mstatus=%x\n", pcb[0].cp->pdir, pcb[0].cp->mstatus);
  printf("(DEBUG)pcb[1]->pdir=%x, mstatus=%x\n", pcb[1].cp->pdir, pcb[1].cp->mstatus);
  printf("go to %d\n", current == &pcb[0]? 0:1);
  assert(pcb[0].cp->pdir == NULL);
  return current->cp;
}
