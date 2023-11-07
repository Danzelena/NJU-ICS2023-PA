#include "elfloader.h"

int cnt = 0;
void ftrace_call(word_t pc){
    for(int i = 0;i < cnt;i++){
        printf(" ");
    }
    printf("%s\n",gen_func_name(pc));

}