#include <common.h>

void init_mm(void);
void init_device(void);
void init_ramdisk(void);
void init_irq(void);
void init_fs(void);
void init_proc(void);

int main() {
  extern const char logo[];
  printf("%s", logo);// 打印 Project-N logo
  Log("'Hello World!' from Nanos-lite");
  Log("Build time: %s, %s", __TIME__, __DATE__);

  init_mm();

  init_device();//目前init_device()会直接调用ioe_init()

  init_ramdisk();// init 磁盘

#ifdef HAS_CTE
  init_irq();
#endif

  init_fs();// 初始化文件系统

  init_proc();// 创建进程

  Log("Finish initialization");

#ifdef HAS_CTE
  yield();
#endif

  panic("Should not reach here");
}
