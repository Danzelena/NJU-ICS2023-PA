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

#include <isa.h>
#include <cpu/cpu.h>
#include <memory/paddr.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"


static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
int call_new_WP();
void del_WP();
void print_wp_pool();
// static word_t paddr_read();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

	// clean something already there
  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);//which add_history?
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);//when encounts 'c',just run it!
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_END;
  cpu_exec(-1);
  return 0;//'q' for quit
}

static int cmd_help(char *args);
static int cmd_si(char *args);
static int cmd_x(char *args);
static int cmd_info(char *args);
static int cmd_p(char *args);
static int cmd_w(char *args);
static int cmd_d(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);//handler is a pointer which
													//turn to a function that input
													//in char and output int
} cmd_table [] = {
  { "help", "Display information about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
	{"si","let program goes N steps then stop,default N=1",cmd_si},
	{"info","print register and watchpoint state",cmd_info},
	{"x","eval the expr ,scan the corrsponding memory",cmd_x},
  {"p","eval the expr",cmd_p},
	{"w","if value of expr changed,stop",cmd_w},
	{"d","del the N.th watchpoint",cmd_d},
					/*TODO*/
};
#define NR_CMD ARRLEN(cmd_table)
int isValid(char*);



static int cmd_si(char *args){
			char *arg = strtok(NULL," ");
		  uint64_t num_step = 1;//default 1	
			if (arg == NULL){
							cpu_exec(num_step);
			}
			else if(isValid(arg)){
				     	num_step = strtol(arg,NULL,10);	
							cpu_exec(num_step);
			}
			else{
							printf("Unknown parameter'%s'\n",arg);
			}
			return 0;
}

static int info_r();
static int info_w();
static int cmd_info(char *args){
				char *arg = strtok(NULL, " ");
				int num_arg = 2;//number of args in arg_table
				struct{
								const char *name;
								const char *description;
								int (*handler)();
				}arg_table [] = {
								{"r","infomation of register",info_r},
								{"w","infomation of watchpoint",info_w},
				};
				int i;
				if(arg == NULL){
								for (i=0;i < num_arg;i ++){
											 printf("%s - %s\n",arg_table[i].name,arg_table[i].description);
																			 }
								 }
				else{
			for(i = 0;i < num_arg;i++){
			if (strcmp(arg,arg_table[i].name)== 0){
			arg_table[i].handler();
			
			}
			}
				}
        return 0;
}
								
static int info_r(){
  // print the register in this function
isa_reg_display();
return 0;
}

static int info_w(){
  print_wp_pool();
  return 0;
}

// finish ugly
static int cmd_x(char *args){
	char *arg_n = strtok(NULL, " ");
	char *arg = strtok(NULL," ");
	if (arg == NULL || arg_n == NULL){
	printf("cmd_x needs a valid expressionto deal with\n");
	return 0;
  }
	else{
    int n;
    n = atoi(arg_n);
    // assume expr is a 0x...
    paddr_t expr_val;
    expr_val = strtol(arg,NULL,16);
    // printf("Debug:n is %d,expr_val is %d\n",n,expr_val);
    if (expr_val < 0x80000000 || expr_val > 0x87ffffff){
      printf("address = 0x%.8x is out of bound of pmem [0x80000000, 0x87ffffff]",expr_val);
    }
    for(int i=0;i< n;i++){
      // printf("Debug:i is %d\n",i);
      if(i % 4 == 0){
        printf("\n");
        // printf("%d\n",expr_val+i*4);
        printf("0x%x :",expr_val+i*4);
      }
      
      for(int j = 0;j <4;j++){
        // word_t re = paddr_read(expr_val+i*4+j,1);
        // printf("%.2x",re);
        uint8_t *pos = guest_to_host(expr_val+i*4+j);
        // *pos += 1;
        printf("0x%.2x  ",*pos);
      }
      printf("\t");
      
    }
    printf("\n");
    return 0;
  
	}
return 0;
}


static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
	/*give the help information of 'arg'*/
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_p(char *args){
  bool su = true;
  bool *success = &su;
  if (*success){
    int ret = expr(args,success);
    printf("%d\n",ret);
  return ret;

  }else{
    printf("invalid expression: %s",args);
    assert(0);
    return 0;
  }

}

/* set watchpoints */
static int cmd_w(char *args){
  bool suc = true;
  call_new_WP(args, &suc);
  if (!suc){
    Log("Fail to do new_WP!\n");
    assert(0);
  }
  return 0;
}

/* delete watchpoints */
static int cmd_d(char *args){
  int no = atoi(args);
  del_WP(no);
  return 0;
}
void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
		/*track the string*/
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");//first call 'strtok'
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
		/*don't found arguments*/
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
/* check the instrucion runs*/
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

int isValid(char *str){
				int isnumber = 1;
				for (int i=0;i < strlen(str);i++){
								if(!isdigit(str[i])){
												isnumber = 0;
								}
				}
				return isnumber;
}
void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
