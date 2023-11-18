#include <elf.h>
#include <common.h>
#include <memory/paddr.h>
#include "ftrace.h"

void init_ftrace(const char *fn);
char *gen_func_name(uint32_t addr);