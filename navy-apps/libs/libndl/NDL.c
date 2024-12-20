#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <regex.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

// 以毫秒为单位返回系统时间
uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return tv.tv_sec*1000+tv.tv_usec/1000;
  // return 0;
}

// 读出一条事件信息, 将其写入`buf`中, 最长写入`len`字节
// 若读出了有效的事件, 函数返回1, 否则返回0
int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events",0,0);
  int flag = read(fd,buf,len);
  int ret = 0;
  if (flag > 0){
    ret = 1;
  }else{
    ret = 0;
  }
  close(fd);
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
  // int fd = open("/dev/fb", 0);
  // for (int i = 0;i < h;i ++){
  //   lseek(fd,((y + i)*screen_w + x)*sizeof(uint32_t),SEEK_SET);
  //   int s = write(fd,pixels + w * i,w*sizeof(uint32_t));
  // }
  int graphics = open("/dev/fb", 0);
  int canvas_x = 0;
  int canvas_y = 0;
  // printf("Debug:x=%d,y=%d\n",x,y);

  for (int i = 0; i < h; ++i){
    lseek(graphics, ((canvas_y + y + i) * screen_w + (canvas_x + x)) * sizeof(uint32_t), SEEK_SET);
    // printf("offset1=%d\n",((canvas_y + y + i) * screen_w + (canvas_x + x)) * sizeof(uint32_t));
    ssize_t s = write(graphics, pixels + w * i, w * sizeof(uint32_t));
  }
  // BUG:not close fild
  // close(fd);
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

// Warning: because of KISS rule(
void extract_values(const char *content) {
  char *token;
  // char *rest = malloc(256);
  char *rest = strdup(content);  // 复制content，以便保留原始内容
  int cnt = 0;
  while ((token = strtok_r(rest, ":", &rest))) {
      // 去除空白字符
      char *trimmed_value = strtok(token, " \t\n\r");
      // if (trimmed_value != NULL) {
        
      //   // printf("Value: %s\n", trimmed_value);
      // }
      if(cnt == 1){
        screen_w = atoi(trimmed_value);
      }else if(cnt == 2){
        screen_h = atoi(trimmed_value);
      }else if(cnt == 3){
        return;
      }
      cnt++;
  }
  // printf("Debug:cnt=%d\n,",cnt);
  // free(rest);  // 释放内存
}
// void extract_values2(const char *str){
//   regex_t regex;
//   regmatch_t matches[2];
  
//   int ret = regcomp(&regex, "\\[\\w+\\] : \\[(\\d+)\\]", REG_EXTENDED);
//   if (ret != 0) {
//         char error_buffer[100];
//         regerror(ret, &regex, error_buffer, sizeof(error_buffer));
//         fprintf(stderr, "Regex compilation failed: %s\n", error_buffer);
//         exit(EXIT_FAILURE);
//     }

//   // 进行匹配
//   ret = regexec(&regex, str, 2, matches, 0);

//   // 处理匹配结果
//   if (ret == 0) {
//       // 提取匹配到的value
//       int start = matches[1].rm_so;
//       int end = matches[1].rm_eo;
//       char value_str[10];  // 假设value的最大长度为10

//       strncpy(value_str, str + start, end - start);
//       value_str[end - start] = '\0';

//       // 转换为整数
//       int value_as_int = atoi(value_str);
//       printf("Value as int: %d\n", value_as_int);
//   } else if (ret == REG_NOMATCH) {
//       fprintf(stderr, "No match found.\n");
//   } else {
//       char error_buffer[100];
//       regerror(ret, &regex, error_buffer, sizeof(error_buffer));
//       fprintf(stderr, "Regex matching failed: %s\n", error_buffer);
//   }

//   // 释放正则表达式资源
//   regfree(&regex);
// }


int NDL_Init(uint32_t flags) {
  printf("(Debug)(NDL_Init)begin!\n");

  // BUG: incorrect envp make Segementation fault!!!
  // 经过排查,只需要在crt0中不修改environment变量即可
  char *test = getenv("Path");
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  printf("(Debug)(NDL_init)after getenv\n");

  /* read /proc/dispinfo */
  int dispinfo = open("/proc/dispinfo",0,0);
  if(dispinfo < 0){
    // panic("fail to open\n");
  }
  char buf[128];
  int flag = read(dispinfo,buf,sizeof(buf));
  if(flag <= 0){
    // panic("fail to read\n");
  }
  /* handle the buf use ragex */

  extract_values(buf);
  // printf("Width:%d,Height:%d\n",screen_w,screen_h);
  printf("NDL_init return\n");
  return 0;
}

void NDL_Quit() {
  printf("NDL_Quit!\n");
  exit(0);// for test
}
