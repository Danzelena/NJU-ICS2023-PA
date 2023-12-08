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

size_t dispinfo_read(void *buf, size_t offset, size_t len)
{
  return 0;
}

size_t fb_write(const void *buf, size_t offset, size_t len)
{
  return 0;
}

void init_device()
{
  Log("Initializing devices...");
  ioe_init();
}
