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
#include "local-include/reg.h"

const char *regs[] = {
    "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"};

const char *sr[] = {
    "mtvec", "mepc", "mstatus", "mcause", "satr"};
void isa_reg_display()
{
  int i;
  int reg_num = 32;
  for (i = 0; i < reg_num; i++)
  {
    // char *name = regs[i];
    bool suc = true;
    word_t val = cpu.gpr[i];
    assert(suc);
    printf("%s\t:%x\n", regs[i], val);
  }
  printf("mtvec\t:%x\n", cpu.sr[mtvec]);
  printf("mepc\t:%x\n", cpu.sr[mepc]);
  printf("mstatus\t:%x\n", cpu.sr[mstatus]);
  printf("mcause\t:%x\n", cpu.sr[mcause]);
}
word_t set_bit(word_t reg, size_t dist_off, size_t src){
  word_t ret;
  switch (src)
  {
  case 1:
    ret = reg | (1 << dist_off);
    break;
  case 0:
    ret = reg & ~(1 << dist_off);
    break;
  default:
    printf("src=%lx\n", src);
    panic("Should not reach here\n");
    break;
  }
  return ret;
}
// return name=s register's value, and if success
word_t isa_reg_str2val(const char *s, bool *success)
{
  *success = true;
  if (strcmp(s, regs[0]) == 0)
  {
    return cpu.gpr[0];
  }

  for (int i = 1; i < ARRLEN(regs); ++i)
  {
    if (strcmp(regs[i], s) == 0)
    { // 跳过$
      *success = true;
      return cpu.gpr[i];
    }
  }
  for (int i = 1; i < 4; ++i)
  {
    if (strcmp(sr[i], s) == 0)
    {
      //"mtvec", "mepc", "mstatus", "mcause"
      *success = true;
      switch (i)
      {
      case 0:
        return cpu.sr[mtvec];
        break;
      case 1:
        return cpu.sr[mepc];
        break;
      case 2:
        return cpu.sr[mstatus];
        break;
      case 3:
        return cpu.sr[mcause];
        break;
      default:
        break;
      }
    }
  }
  printf("Sorry:ail to find this register!");
  *success = false;
  return 0;
}
