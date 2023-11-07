#include "elfloader.h"

void ftrace_call(word_t pc){
    printf("%s\n",gen_func_name(pc));

}