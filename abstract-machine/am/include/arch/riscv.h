#ifndef ARCH_H__
#define ARCH_H__

#ifdef __riscv_e
#define NR_REGS 16:
#else
#define NR_REGS 32
#endif

struct Context {
  // tODO: fix the order of these members to match trap.S
  uintptr_t gpr[NR_REGS], mcause, mstatus,mepc;
  // uintptr_t mepc, mcause, gpr[NR_REGS], mstatus;
  void *pdir;
  uintptr_t np;
};

#ifdef __riscv_e
#define GPR1 gpr[15] // a5
#else
#define GPR1 gpr[17] // a7
#endif

#define GPR2 gpr[10] // a0
#define GPR3 gpr[11] // a1
#define GPR4 gpr[12] // a2
// #define GPR2 gpr[0]
// #define GPR3 gpr[0]
// #define GPR4 gpr[0]
#define GPRx gpr[10] // a0

#define GPRra gpr[2] // ra
#define GPRa0 gpr[10]
#define GPRa1 gpr[11]
#define GPRa2 gpr[12]
#define GPRa3 gpr[13]
#define GPRa4 gpr[14]
#define GPRa5 gpr[15]
#define GPRa6 gpr[16]
#define GPRa7 gpr[17]

#endif
