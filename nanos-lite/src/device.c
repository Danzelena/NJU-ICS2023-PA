#include <common.h>
/* 设备抽象 */
#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
#define MULTIPROGRAM_YIELD() yield()
#else
#define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
    [AM_KEY_NONE] = "NONE",
    AM_KEYS(NAME)};

size_t serial_write(const void *buf, size_t offset, size_t len)
{
  // call putch()
  for (int i = 0; i < len && ((char *)buf)[i] != '\0'; i++)
  {
    putch(((char *)buf)[i]);
  }
  return 0;
}

size_t events_read(void *buf, size_t offset, size_t len)
{
  bool has_uart,has_kbd;
  has_uart = io_read(AM_UART_CONFIG).present;
  has_kbd  = io_read(AM_INPUT_CONFIG).present;
  if(!has_uart && !has_kbd){
    panic("not have uart or kbd\n");
  }
  if (has_uart) {
    panic("has_uart!\n");
    // while (1) {
    //   char ch = io_read(AM_UART_RX).data;
    //   if (ch == -1) break;
    //   printf("Got (uart): %c (%d)\n", ch, ch & 0xff);
    // }
  }
  int ret = 0;
  if (has_kbd) {
    
    AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
    // printf("Got  (kbd): %s (%d) %s\n", keyname[ev.keycode], ev.keycode, ev.keydown ? "DOWN" : "UP");
    // 若当前没有有效按键, 则返回0即可.
    if (ev.keycode == AM_KEY_NONE){
      return 0;
    }
    if (ev.keydown){
      ret = snprintf(buf,len,"kd %s\n",keyname[ev.keycode]);
      if(ret >= len){
        panic("the output was truncated\n");
      }
    }else{
      ret = snprintf(buf,len,"ku %s\n",keyname[ev.keycode]);
      if(ret >= len){
        panic("the output was truncated\n");
      }
    }

  }
  return ret;
}

// 不支持lseek,可以忽略offset
// 将文件(/proc/dispinfo,即屏幕大小)的len 字节写到buf 中
// dispinfo 由屏幕大小的获得, 读出的格式为:
// WIDTH : 640
// HEIGHT: 480
size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  int width = io_read(AM_GPU_CONFIG).width;
  int height = io_read(AM_GPU_CONFIG).height;
  int ret = snprintf(buf,len,"WIDTH : %d\nHEIGHT : %d\n",width,height);
  if(ret >= len){
    panic("the output was truncated\n");
  }
  return ret;
  // return 0;
}

// 用于把buf中的len字节写到屏幕上offset处. 你需要先从offset计算出屏幕上的坐标, 然后调用IOE来进行绘图. 另外我们约定每次绘图后总是马上将frame buffer中的内容同步到屏幕上.
size_t fb_write(const void *buf, size_t offset, size_t len)
{
  // BUG:how to implement fb_write
  int width = io_read(AM_GPU_CONFIG).width;
  // int height = io_read(AM_GPU_CONFIG).height;
  // /* 计算坐标 */
  printf("offset=%d,width=%d\n",offset,width);
  int y_ind = offset / width;
  //BUG:Y FIX
  y_ind /= 4;
  int x_ind = offset % width;
  
  printf("Debug:offset2=%d,x_ind=%d,y_ind=%d\n",offset,x_ind,y_ind);
  // void * buf_c = buf;
  // printf("Debug:x=%d,y=%d\n",x_ind,y_ind);
  // /* IOE绘图 */
  // io_write(AM_GPU_FBDREAW,x_ind,y_ind,)

  // /* 内容同步 */
  // return 0;
  // uintptr_t *ptr;
  // ptr = (uintptr_t *[])(&buf);
  // printf("target1\n");
  io_write(AM_GPU_FBDRAW, x_ind, y_ind, buf, len/sizeof(__uint32_t), 1, true);
  // io_write(AM_GPU_MEMCPY, offset, (void *)*ptr, len);
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  
  return len;
}

void init_device()
{
  Log("Initializing devices...");
  ioe_init();
}
