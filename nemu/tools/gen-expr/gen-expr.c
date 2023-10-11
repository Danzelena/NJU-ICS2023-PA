/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#define MAX_LEN 30
int number_range = 10;
// this should be enough
static char buf[65536] = {};
// int buf_index;
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

uint32_t choose(uint32_t n){
  return rand() % n;
}
static void gen(char ch){

  char cha =ch;
  strcat(buf,&cha);


  
}
static void gen_rand_op(){
  char op;
  switch (choose(3))
  {case 0:op = '+';break;
  case 1:op = '-';break;
  case 2:op = '*';break;
  case 3:op = '/';break;
  }
  gen(op);
  }

static void gen_num(){
  int num = choose(number_range);
  char ch = ' ';
  char *num_str = &ch;
  
  sprintf(num_str,"%d",num);
  strcat(buf,num_str);
}
static void gen_rand_expr() {
  // TODO
  switch(choose(3)){
    case 0:{
      gen_num();
      break;
    }case 1:{
      gen('(');
      gen_rand_expr();
      gen(')');
      break;
    }case 2:{
      gen_rand_expr();
      gen_rand_op();
      gen_rand_expr();
      break;
    }default:{
      gen_num();
      break;
    }
  }
  // buf[0] = '\0';
}

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  
  int loop = 1;// times for test
  /*argv[1] is times for test*/
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    // buf_index = 0;
    buf[0] = '\0';
    gen_rand_expr();
    // printf("Debug: %s\n",buf);
    if(strlen(buf) > MAX_LEN){
      break;
    }
    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    ret = fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
