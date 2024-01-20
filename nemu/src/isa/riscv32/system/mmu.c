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
#include <memory/vaddr.h>
#include <memory/paddr.h>

#include "../local-include/reg.h"

#define SATP_PPN 0x3fffff
#define VPN1_MASK 0xffc00000
#define VPN0_MASK 0x3ff000
#define VPN0(x) (((vaddr_t)x & VPN0_MASK)>> 12)
#define VPN1(x) (((vaddr_t)x & VPN1_MASK)>> 22)
#define PTE_V 0x01
#define PTE_A 0x40
#define PTE_D 0x80
#define PTE_PPN_MASK 0xfffffc00
#define PTE_PPN(x) (((vaddr_t)x & PTE_PPN_MASK)>> 10)
#define VA_OFFSET(x) ((vaddr_t)x & 0x00000FFFu)


#define READ 0
#define WRITE 1

typedef uint32_t PTE;
extern riscv32_CPU_state cpu;
// TODO:对内存区间为[vaddr, vaddr + len), 类型为type的内存访问进行地址转换
paddr_t isa_mmu_translate(vaddr_t vaddr, int len, int type) {
  paddr_t satp_ppn = cpu.sr[satp] & SATP_PPN;
  paddr_t pt1_e_addr = satp_ppn *4096 + (VPN1(vaddr)) * 4;
  PTE pt1_e = paddr_read(pt1_e_addr, 4);
  //satp: (should)0x81f96000 (but)81f73000
  Assert(pt1_e & PTE_V, "pt1_e is not valid, satp_ppn: %#x, pt1_e_addr: %#lx, vaddr: %#x", satp_ppn, (unsigned long)pt1_e_addr, vaddr);

  paddr_t pt2_e_addr = PTE_PPN(pt1_e) * 4096 + VPN0(vaddr) * 4;
  PTE pt2_e = paddr_read(pt2_e_addr, 4);
  Assert(pt2_e & PTE_V, "pt2_e is not valid, vaddr: %#x, pt2_e_addr: %#x", vaddr, pt2_e_addr);

  switch (type)
  {
  case MEM_TYPE_IFETCH:case MEM_TYPE_READ:
    paddr_write(pt2_e_addr, 4, pt2_e | PTE_A);
    break;
  case MEM_TYPE_WRITE:
    paddr_write(pt2_e_addr, 4, pt2_e | PTE_D);
  default:
    Assert(0, "Shoule not reach here\n");
    break;
  }
  paddr_t pa = PTE_PPN(pt2_e) * 4096 + VA_OFFSET(vaddr);
  Assert(pa==vaddr, "Vaddr(%#x)!=Paddr(%#x)", vaddr, pa);
  // printf("(MMU)\n va=%x\npa=%x\n", vaddr, pa);
  // Assert(0, "Should stop here\n");
  return pa;
  // return MEM_RET_FAIL;
}
