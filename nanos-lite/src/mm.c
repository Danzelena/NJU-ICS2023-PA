#include <memory.h>
#include <common.h>
static void *pf = NULL;

// TODO:实现 new_page
// 分配一段 nr_page * 4KB 的内存区域,并返回首地址
void* new_page(size_t nr_page) {
  void *ret = pf;
  pf += nr_page * PGSIZE;
  printf("(Debug)pf=0x%x\n", pf);
  return ret;
  // return NULL;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  return NULL;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
