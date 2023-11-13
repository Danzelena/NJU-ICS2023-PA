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
#include <cpu/difftest.h>
#include "../local-include/reg.h"

//*ref_r like: {gpr = {0, 0, 0, 0, 0, 2147483648, 0 <repeats 26 times>}, pc = 2147483652}
// pc : 2147483648
bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  size_t i;
  int reg_num = 32;
  for(i = 0;i < reg_num;i++){
    if((*ref_r).gpr[i] != cpu.gpr[i]){
      return false;
    }
  }
  return true;
  // return false;
}

void isa_difftest_attach() {
}
