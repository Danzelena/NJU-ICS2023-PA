#include <elf.h>
#include <common.h>
#include <memory/paddr.h>

void init_ftrace(const char *fn);
char *gen_func_name(uint64_t addr);