#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>
#include <readline/readline.h>
#include <readline/history.h>

#define MAX_FP 128
#define MAX_INPUT 128
#define MAX_ARGC 10
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
static char _input[MAX_INPUT];// 最长一次输入MAX_INPUT个字符
static char *init_input(const char *str){
  assert(strlen(str) <= MAX_INPUT);
  strncpy(_input, str, MAX_INPUT);
  assert(strlen(_input) == strlen(str));
  _input[strlen(_input) - 1] = '\0';// delete '\n' in the end
  printf("(nterm)get _input=%s\n", _input);
  return _input;
}
// TODO: parse the cmd

// static char *str_handle_cmd(const char *str){
//   assert(str != NULL);
//   // FORWARD:fp size fixed!
//   assert(strlen(str) <= MAX_FP);
//   strcpy(fp,str);
//   fp[strlen(fp)-1] = '\0';
//   printf("get string=%s\n",fp);  
//   return fp;
// }
static char *input_handle_fp(char *input){
  printf("(Debug)input__fp\n");
  
  assert(input != NULL);
  char *input_r = input;
  // strncpy(input_r, input, strlen(input));
  char *fp = strtok(input_r, " ");
  assert(fp != NULL);
  printf("(nterm)get fp=%s, strlen=%d\n", fp, strlen(fp));
  return fp;
}

static char *input_handle_arg(char *input, char *fp, char *end){
  printf("(Debug)input__arg\n");
  assert(input != NULL || fp != NULL);
  // printf("(nterm)arg=%x\n", input);
  // printf("(Debug)arg=%d, input_end=%d\n", arg, input_end);
  // return input;
  
  char *arg = input + strlen(fp) + 1;
  
  printf("(Debug)arg=%d, input_end=%d\n", arg, end);
  if(arg >= end){
    return NULL;
  }
  return arg;
}

static char **arg_handle_argv(char *arg){
  printf("(Debug)arg__argv\n");
  assert(arg != NULL);
  if(arg == NULL){
    return (char **)NULL;
  }
  char *arg_end = arg + strlen(arg);

  char *argv[MAX_ARGC];
  char *arg_r = arg;

  argv[0] = strtok(arg_r, " ");

  int i;
  for (i = 1; ;i++){
    char *token = strtok(NULL, " ");
    if(token == NULL){
      break;
    }else{
      argv[i] = token;
    }
  }

  // char *arg_ptr = NULL;
  // size_t i;
  // for(i = 0;str_ptr <= arg_end;i ++){
  //   if(i == 0){
  //     arg_ptr = strtok(str_ptr, " ");
      
  //   }
  //   // arg_ptr = strtok(str_ptr, " ");
  //   // str_ptr += strlen(arg_ptr);// addi str_ptr
  //   argv[i] = arg_ptr;
  // }
  assert(i <= MAX_ARGC);
  /* check for Debug */
  for(int j = 0;j <= i;j ++){
    printf("(Check)argv[%d]=%s\n", j, argv[j]);
  }
  return argv;
}
// 命令解析函数,把键入的命令作为参数调用 execve()
static void sh_handle_cmd(const char *cmd) {
  char *input = init_input(cmd);
  char *input_end = input + strlen(input);
  char *filepath = input_handle_fp(input);
  char *arg = input_handle_arg(input, filepath, input_end);
  char **argv = arg_handle_argv(arg);

  // char *filepath = str_handle_cmd(cmd);
  assert(0);
  // int flag = execvp(filepath,NULL);
  int flag = execvp(filepath, argv);

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
