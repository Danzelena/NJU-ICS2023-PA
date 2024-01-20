#include <memory.h>
#include <common.h>
static void *pf = NULL;

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
