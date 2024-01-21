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
#include "../local-include/reg.h"

#define IRQ_TIMER 0x80000007
/**
 * parameter:
 * NO: interrupt/exception reason;
 * Epc: current pc
*/
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* tODO: Trigger an interrupt/exception with ``NO''.'
   * Then return the address of the interrupt/exception vector.
   */

  /* 让处理器处于关中断状态 */
  // MPIE <- MIE
  set_bit(cpu.sr[mstatus], MPIE_OFF, MIE(cpu.sr[mstatus]));
  Assert(MPIE(cpu.sr[mstatus]) == MIE(cpu.sr[mstatus]), "MPIE <- MIE");
  // MIE <- 0
  set_bit(cpu.sr[mstatus], MIE_OFF, 0);
  Assert(MIE(cpu.sr[mstatus]) == 0, "MIE <- 0");
  /* exception trace */
  // printf("(etrace)cause:%d,epc:0x%x\n",NO,epc);
  cpu.sr[mepc] = epc;
  cpu.sr[mcause] = NO;
  return cpu.sr[mtvec];


  return 0;
}

word_t isa_query_intr() {
  if(cpu.INTR && MIE(cpu.sr[mstatus])){
    cpu.INTR = false;
    return (word_t)IRQ_TIMER;
  }else{
    return INTR_EMPTY;
  }
  // return cpu.INTR;
  // return INTR_EMPTY;
}
