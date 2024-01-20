#include <am.h>
#include <nemu.h>
#include <klib.h>

#define PTE_V 0x01 // 是否分配
#define PTE_R 0x02 // read
#define PTE_W 0x04 // write
#define PTE_X 0x08 // execve
#define PTE_U 0x10 // U/S mode
#define PTE_A 0x40 // 
#define PTE_D 0x80 // dirty
#define PTE_CHAR_MASK 0x3ff // above all 
#define PTE_PPN_MASK (0xfffffc00)
#define PTE_PPN(x) (((uintptr_t)x & PTE_PPN_MASK) >> 10)

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  /* set call-back function */
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;
  printf("(vme_init)\n");
  kas.ptr = pgalloc_f(PGSIZE);// 一级页表(页目录)的基地址
  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    // printf("va_begin=%x\n", va);
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
    // printf("va_end=%x\n", va);
    
  }

  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

// 创建一个默认的地址空间
void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;// updir是一个页表项
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

// 销毁指定的地址空间
void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }
}
int cnt = 0;
// TODO:riscv32-nemu map()
// 用于将va所在的虚拟页, 以prot的权限映射到pa所在的物理页
void map(AddrSpace *as, void *va, void *pa, int prot) {
  /* check offset */
  if(va==(void*)0x40000000){
    printf("(MAP)start, va=0x%x, pa=0x%x\n", va, pa);
  }
  
  // printf("va=%x\n", va);
  uintptr_t va_offset = (uintptr_t)va & 0xfff;
  uintptr_t pa_offset = (uintptr_t)pa & 0xfff;
  assert(va_offset == pa_offset);

  uintptr_t vpn1 = ((uintptr_t)va & 0xffc00000)>> 22;
  uintptr_t vpn0 = ((uintptr_t)va & 0x3ff000)>> 12;
  uintptr_t ppn = ((uintptr_t)pa & (~0xfff))>> 12;

  /* 一级页表 */
  assert(as->ptr);
  PTE *pt1_e = (uintptr_t*)(as->ptr + (vpn1 <<2));
  if(va==(void*)0x40000000){
    printf("(MAP)pt1_e=%x\n", pt1_e);
  }
  // assert((uintptr_t)pt1_e == get_satp() + vpn1 * 4);

  /* 查看二级页表是否分配 */
  assert(pt1_e);
  if(!(*pt1_e & PTE_V)){
    void *allocated_pt2 = pgalloc_usr(PGSIZE);
    printf("(MAP)allocated_pt2=%x\n", allocated_pt2);
    /* 写入 pt1_e */
    *pt1_e &= ~PTE_PPN_MASK;
    *pt1_e |= ((uintptr_t)allocated_pt2 >> 2 & PTE_PPN_MASK);
    *pt1_e = (*pt1_e & ~PTE_PPN_MASK) | (((uintptr_t)allocated_pt2 >> 2) & (PTE_PPN_MASK));
    *pt1_e |= PTE_V;
    assert(((PTE_PPN(*pt1_e) * 4096 + vpn0 * 4) & ~0xFFFFFF) == ((uintptr_t)allocated_pt2 & ~0xFFFFFF));

  }
  /* 二级页表 */ 
  PTE *pt2_e = (uintptr_t*)(PTE_PPN(*pt1_e) * 4096 + vpn0 * 4);
  *pt2_e = (ppn << 10) | (PTE_V) | (PTE_R) | (PTE_W) | (PTE_X);
  if(prot){
    *pt2_e |= PTE_U;
  }
  // printf("(MAP)\n va=%x\n pa=%x\n", va, pa);
  // assert(0);
}

// similar to kcontext() but for user prog
// as -> 限制用户进程可以访问的内存
// kstack -> 内核栈, 用于分配上下文结构
// entry -> 用户进程的入口
Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  // printf("(Debug)begin to ucontext\n");
  Context *context = kstack.end - sizeof(Context);
  // printf("(Debug)context=%x\n", context);
  context->mstatus = 0x1800;
  context->mepc = (uintptr_t)entry;
  context->pdir = as->ptr;
  // context->GPRx = (uintptr_t)heap.end;
  // printf("(Debug)context->mepc=%x\n", context->mepc);
  return context;
  // return NULL;
}
