#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

// 以毫秒为单位返回系统时间
uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec*1000000+tv.tv_usec;
  return 0;
}

// 读出一条事件信息, 将其写入`buf`中, 最长写入`len`字节
// 若读出了有效的事件, 函数返回1, 否则返回0
int NDL_PollEvent(char *buf, int len) {
  int fd = _open("/dev/events",0,0);
  int flag = _read(fd,buf,len);
  int ret = 0;
  if (flag > 0){
    ret = 1;
  }else{
    ret = 0;
  }
  _close(fd);
  return ret;
}

// 打开一张(*w) X (*h)的画布
// 如果*w和*h均为0, 则将系统全屏幕作为画布, 并将*w和*h分别设为系统屏幕的大小
void NDL_OpenCanvas(int *w, int *h) {

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

// 向画布`(x, y)`坐标处绘制`w*h`的矩形图像, 并将该绘制区域同步到屏幕上
// 图像像素按行优先方式存储在`pixels`中, 每个像素用32位整数以`00RRGGBB`的方式描述颜色
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  printf("NDL_Init!\n");
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  /* read /proc/dispinfo */
  int dispinfo = _open("/proc/dispinfo",0,0);
  if(dispinfo < 0){
    // panic("fail to open\n");
  }
  char buf[128];
  int flag = _read(dispinfo,buf,sizeof(buf));
  if(flag <= 0){
    // panic("fail to read\n");
  }

  /* handle the buf use ragex */
  char line[64];
  while (fgets(line, sizeof(line), dispinfo) != NULL) {
      char key[32];
      char value[32];

      // 使用sscanf从字符串中提取key和value
      if (sscanf(line, "[%99[^]]] : [%99[^]]]", key, value) == 2) {
        // 去除key和value两端的空白字符
        char *trimmed_key = strtok(key, " \t\n\r");
        char *trimmed_value = strtok(value, " \t\n\r");

        printf("Key: %s, Value: %s\n", trimmed_key, trimmed_value);
      }
  }
  return 0;
}

void NDL_Quit() {
  printf("NDL_Quit!\n");
}
