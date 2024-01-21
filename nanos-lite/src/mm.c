/* a program break is end of the process's data segment*/

#include <memory.h>
#include <common.h>
#include <proc.h>

#define VPN_OFF 12
static void *pf = NULL;
extern PCB *current;

// 实现 new_page(物理页)
// 分配一段 nr_page * 4KB 的内存区域,并返回首地址
void* new_page(size_t nr_page) {
  void *ret = pf;
  pf += nr_page * PGSIZE;
  // printf("(Debug)pf=0x%x\n", pf);
  return ret;
  // return NULL;
}

// TODO: 分配n个字节的空间
#ifdef HAS_VME
static void* pg_alloc(int n) {
  printf("(Debug)(pg_alloc)\n");
  assert(n % PGSIZE == 0);
  void *ret = new_page((int)(n / PGSIZE));
  memset(ret, 0, n);
  printf("begin=%x, end=%x\n", ret, ret + n);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  #ifndef HAS_VME
    return 0;
  #endif
  int prog = 1;
  uintptr_t max_brk = current->max_brk;
  uintptr_t brk_pn = brk >> VPN_OFF;
  uintptr_t max_brk_pn = max_brk >> VPN_OFF;

  if (brk_pn >= max_brk_pn){
    // printf("(Debug)(mm_brk)allocate new page at brk=%x, max_brk=%x\n", brk, max_brk);
    /* alloc new physicsal page */
    size_t new_pn = brk_pn + 1 - max_brk_pn;
    void *ret = new_page(new_pn);
    memset(ret, 0, new_pn * PGSIZE);
    for(size_t i = 0; i < new_pn; i++){
      map(&current->as, (void *)(max_brk + i * (PGSIZE)), (void *)(ret + i * (PGSIZE)), prog);
      // printf("(map)vaddr=%x\n", max_brk + i * (PGSIZE));
    }
    current->max_brk = (brk_pn + 1)<< VPN_OFF;
    assert(current->max_brk > brk);
  }
  
  return 0;
}

void init_mm() {
  /* 将堆区起始地址作为空闲物理页的首地址 */
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
