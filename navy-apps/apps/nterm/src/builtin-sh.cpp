#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

#define MAX_FP 128
char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
    printf("After sh_printf\n");

}

static void sh_prompt() {
  sh_printf("sh> ");
}
static char fp[MAX_FP];// file variable is very important

// TODO: parse the parameter
// delete '\n' in the end
static char *str_handle_cmd(const char *cmd){
  // FORWARD:fp size fixed!
  assert(strlen(cmd) <= MAX_FP);
  strcpy(fp,cmd);
  fp[strlen(fp)-1] = '\0';
  printf("fp=%s\n",fp);
  assert(0);
  return fp;
}
// 命令解析函数,把键入的命令作为参数调用 execve()
static void sh_handle_cmd(const char *cmd) {
  char *filepath = str_handle_cmd(cmd);
  int flag = execvp(filepath,NULL);
  // printf("666\n");
  // int flag = execve(filepath,NULL,NULL);
  if(flag == -1){
    sh_printf("Path to %s not found!\n",filepath);
  }
}

void builtin_sh_run() {
  // printf("shell1\n");
  sh_banner();
  // printf("shell2\n");
  sh_prompt();
  // printf("shell3\n");
  setenv("PATH", "/bin", 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          //input: helloworld
          //then res is 'hello world'
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
