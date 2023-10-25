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

#include "local-include/reg.h"
#include <cpu/cpu.h>
#include <cpu/ifetch.h>
#include <cpu/decode.h>

#define XLEN 32
#define R(i) gpr(i)
#define Mr vaddr_read
#define Mw vaddr_write

enum {
  //TODO:use R2 replace R in order to avoid define R(i)
  TYPE_I, TYPE_U, TYPE_S, TYPE_J, TYPE_B,  TYPE_R2,
  TYPE_N, // none
};
// get val in Register to src
#define src1R() do { *src1 = R(rs1); } while (0)
#define src2R() do { *src2 = R(rs2); } while (0)
// get imm val in inst
#define immI() do { *imm = SEXT(BITS(i, 31, 20), 12); } while(0)
#define immU() do { *imm = SEXT(BITS(i, 31, 12), 20) << 12;} while(0)
#define immS() do { *imm = (SEXT(BITS(i, 31, 25), 7) << 5) | BITS(i, 11, 7);} while(0)
#define immJ() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 20) | BITS(i, 19, 12) << 12 | BITS(i, 20, 20) << 11 | BITS(i, 30, 21) << 1)&~1 ;} while(0)
#define immB() do { *imm = ((SEXT(BITS(i, 31, 31), 1) << 12) | BITS(i, 7, 7) << 11 | BITS(i, 30, 25) << 5 | BITS(i, 11, 8) << 1)&~1;  }while(0)

static void decode_operand(Decode *s, int *rd, word_t *src1, word_t *src2, word_t *imm, int type) {
  uint32_t i = s->isa.inst.val;
  int rs1 = BITS(i, 19, 15);
  int rs2 = BITS(i, 24, 20);
  *rd     = BITS(i, 11, 7);
  switch (type) {
    case TYPE_I: src1R();          immI(); break;
    case TYPE_U:                   immU(); break;
    case TYPE_S: src1R(); src2R(); immS(); break;
    case TYPE_J:                   immJ(); break;
    case TYPE_B: src1R(); src2R(); immB(); break;
    case TYPE_R2: src1R(); src2R();         break;
  }
}

static int decode_exec(Decode *s) {
  int rd = 0;
  // stand for:
  // rd->目的操作数的寄存器号码
  word_t src1 = 0, src2 = 0, imm = 0;
  s->dnpc = s->snpc;
// decode op and operand
#define INSTPAT_INST(s) ((s)->isa.inst.val)
#define INSTPAT_MATCH(s, name, type, ... /* execute body */ ) { \
  decode_operand(s, &rd, &src1, &src2, &imm, concat(TYPE_, type)); \
  __VA_ARGS__ ; \
}

// test files
//   *add.c           *fact.c        *matrix-mul.c    *pascal.c       string.c
// *add-longlong.c  *fib.c         *max.c           *prime.c        sub-longlong.c
// bit.c           *goldbach.c    *mersenne.c      *quick-sort.c   *sum.c
// *bubble-sort.c   hello-str.c   *min3.c          *recursion.c    *switch.c
// crc32.c         *if-else.c     *mov-c.c         *select-sort.c  *to-lower-case.c
// *div.c           *leap-year.c   *movsx.c         shift.c        unalign.c
// *dummy.c         load-store.c  mul-longlong.c  *shuixianhua.c  *wanshu.c

  // instruction pattern
  // format:
  // (string,instruction name,type,operation)
  INSTPAT_START();
  INSTPAT("??????? ????? ????? ??? ????? 00101 11", auipc  , U, R(rd) = s->pc + imm);
  INSTPAT("??????? ????? ????? 100 ????? 00000 11", lbu    , I, R(rd) = Mr(src1 + imm, 1));
  INSTPAT("??????? ????? ????? 000 ????? 01000 11", sb     , S, Mw(src1 + imm, 1, src2));
  // add:
  // li addi jal ret sw jal mv ebreak j
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", addi   , I, R(rd) = src1 + imm);
  INSTPAT("??????? ????? ????? ??? ????? 11011 11", jal    , J, R(rd) = s->pc + 4; s->dnpc = s->pc + imm);
  INSTPAT("??????? ????? ????? 000 ????? 11001 11", jalr   , I, R(rd) = s->pc + 4;s->dnpc = ((imm + src1)&~1));
  INSTPAT("??????? ????? ????? 010 ????? 01000 11", sw     , S, Mw(src1 + imm, 4, src2));
  // TODO:finish these instruction
  // lw,bge,sub,seqz,bne,...
  INSTPAT("??????? ????? ????? 010 ????? 00000 11", lw     , I, R(rd) = Mr((src1 + imm), 4));// '4' for a 32bits-word in riscv32
  INSTPAT("??????? ????? ????? 101 ????? 11000 11", bge    , B, if((sword_t)src1 >=(sword_t)src2){ s->dnpc = s->pc + imm;});
  INSTPAT("0100000 ????? ????? 000 ????? 01100 11", sub    , R2, R(rd) = src1 - src2);
  INSTPAT("??????? ????? ????? 000 ????? 11000 11", beq    , B, if(src1 == src2){s->dnpc = s->pc + imm;});
  INSTPAT("??????? ????? ????? 011 ????? 00100 11", sltiu  , I, if((word_t)src1 < (word_t)imm){R(rd) = 1;}else{R(rd) = 0;});
  INSTPAT("??????? ????? ????? 001 ????? 11000 11", bne    , B, if(src1 != src2){s->dnpc = s->pc + imm;});
  // finish max

  //add
  INSTPAT("0000000 ????? ????? 000 ????? 01100 11", add    , R2, R(rd) = src1 + src2);
  // finish min3,add

  //mul, div
  INSTPAT("0000001 ????? ????? 000 ????? 01100 11", mul    , R2, R(rd) = src1 * src2);
  INSTPAT("0000001 ????? ????? 100 ????? 01100 11", div    , R2, R(rd) = (word_t)src1 / (word_t)src2);
  // finish div

  // lui, sra, srai, sll, slli
  INSTPAT("??????? ????? ????? ??? ????? 01101 11", lui    , U, R(rd) = imm);
  INSTPAT("0100000 ????? ????? 101 ????? 01100 11", sra    , R2, R(rd) = (sword_t)src1 >> (src2 & 0b11111));
  INSTPAT("010000? ????? ????? 101 ????? 00100 11", srai   , I, R(rd) = (sword_t)src1 >> (imm & 0b111111));
  INSTPAT("0000000 ????? ????? 001 ????? 01100 11", sll    , R2, R(rd) = src1 << src2);
  INSTPAT("000000? ????? ????? 001 ????? 00100 11", slli   , I, R(rd) = src1 << (imm & 0b111111) );
  // finish recursion

  // blt, bltu, slt, and, andi
  INSTPAT("??????? ????? ????? 100 ????? 11000 11", blt    , B, if((sword_t)src1 < (sword_t)src2){s->dnpc = s->pc + imm;});
  INSTPAT("??????? ????? ????? 110 ????? 11000 11", bltu   , B, if((word_t)src1 < (word_t)src2){s->dnpc = s->pc + imm;});
  INSTPAT("0000000 ????? ????? 010 ????? 01100 11", slt    , R2, if((sword_t)src1 < (sword_t)src2){R(rd) = 1;}else{R(rd) = 0;});
  INSTPAT("0000000 ????? ????? 111 ????? 01100 11", and    ,R2, R(rd) = src1 & src2);
  INSTPAT("??????? ????? ????? 111 ????? 00100 11", andi   , I, R(rd) = src1 & imm);
  // finish if-else

  // rem, remu, mulh, xor, sltu, divu
  INSTPAT("0000001 ????? ????? 110 ????? 01100 11", rem    , R2, R(rd) = (sword_t)src1 % (sword_t)src2);
  INSTPAT("0000001 ????? ????? 111 ????? 01100 11", remu   ,R2, R(rd) = (word_t)src1 % (word_t)src2);
  INSTPAT("0000001 ????? ????? 001 ????? 01100 11", mulh   ,R2, R(rd) = (uint64_t)((sword_t)src1 * (sword_t)src2)>>XLEN);
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    ,R2, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 011 ????? 01100 11", sltu   ,R2, if((word_t)src1 < (word_t)src2){R(rd) = 1;}else{R(rd) = 0;});
  INSTPAT("0000001 ????? ????? 101 ????? 01100 11", divu   ,R2, R(rd) = (word_t)src1 / (word_t)src2);
  // finish mersenne, pascal

  // or, xor, ori, xori
  INSTPAT("0000000 ????? ????? 100 ????? 01100 11", xor    , R2, R(rd) = src1 ^ src2);
  INSTPAT("0000000 ????? ????? 110 ????? 01100 11", or     , R2, R(rd) = src1 | src2);
  INSTPAT("??????? ????? ????? 110 ????? 00100 11", ori    , R2, R(rd) = src1 | imm);
  INSTPAT("??????? ????? ????? 000 ????? 00100 11", xori   , I, R(rd) = src1 ^ imm);

  INSTPAT("0000000 00001 00000 000 00000 11100 11", ebreak , N, NEMUTRAP(s->pc, R(10))); // R(10) is $a0
  INSTPAT("??????? ????? ????? ??? ????? ????? ??", inv    , N, INV(s->pc));
  INSTPAT_END();

  R(0) = 0; // reset $zero to 0

  return 0;
}

int isa_exec_once(Decode *s) {
  // will modift s->snpc,so snpc is next inst's pc
  s->isa.inst.val = inst_fetch(&s->snpc, 4);
  return decode_exec(s);
}
