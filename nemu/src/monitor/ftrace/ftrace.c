#include "elfloader.h"

// int cnt = 0;
// char *lastfunc;
// TODO:simluate a function call stack to print
void ftrace_call(word_t pc){
    // for(int i = 0;i < cnt;i++){
    //     printf(" ");
    // }
    char *func = gen_func_name(pc);
    
    printf("%s\n",func);
    // if()
    

}