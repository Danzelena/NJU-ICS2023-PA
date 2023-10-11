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

#include <common.h>

void init_monitor(int, char *[]);
void am_init_monitor();
void engine_start();
int is_exit_status_bad();

#define CONFIG_DEBUG 1
#define INPUT_PATH "../tools/gen-expr/input"

int test_cmd_q();
int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
#ifdef CONFIG_TARGET_AM
  am_init_monitor();
#else
  init_monitor(argc, argv);
#endif 
  IFDEF(CONFIG_DEBUG,test_cmd_q());

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
int test_cmd_q(){
  // read input file
  char buff[255];
  char opt[255];
  FILE *input = NULL;

  input = fopen(INPUT_PATH,"r");
  if(input == NULL){
    printf("can't read file:input!\n");
    assert(0);
  }
  // read output
  if(fscanf(input,"%s",opt) == 1){
  printf("output:%s",opt);

  }  


  // read expr
  if(fgets(buff,255,input)!= NULL){
  printf("expr:%s\n",buff);

  }


  fclose(input);
  // calculate expr

  // compare output and answer
  return 0;

}