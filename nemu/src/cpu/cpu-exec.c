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

#include <cpu/cpu.h>
#include <cpu/decode.h>
#include <cpu/difftest.h>
#include <locale.h>
#include "../monitor/sdb/sdb.h"
#include "../monitor/ftrace/elfloader.h"

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INST_TO_PRINT 10
#define IRING_LEN 15
#define STR_LEN 50

CPU_state cpu = {};
uint64_t g_nr_guest_inst = 0;
static uint64_t g_timer = 0; // unit: us
static bool g_print_step = false;

struct iringbuf{
  char ** buf_ptr;
  size_t read_index;
  size_t write_index;
  size_t buf_size;
}irbuf;
char *pool[IRING_LEN];


// tODO:handle iringbuf as a queue
void irbuf_print();
void irbuf_init(struct iringbuf *rb, 
                  char **pool,
                  size_t size){
  /*initialize read and write index*/
  rb->read_index = 0;
  rb->write_index = 5;

  /*set buffer pool and size*/
  

  for (size_t i = 0;i < size;i++){
    // *(rb->buf_ptr + i) = NULL;
    pool[i] = (char *)malloc(STR_LEN*sizeof(char));
    snprintf(pool[i], STR_LEN, "(no instruction)");
    // memset(pool[i], '\0', STR_LEN);
  }
  rb->buf_ptr = pool;
  rb->buf_size = size;
  // irbuf_print(rb);
}

size_t irbuf_push(struct iringbuf *rb, char *inst){
  size_t ret = rb->write_index;
  // printf("Debug:write_index = %d\n", (int)rb->write_index);
  // Log("before push inst");
  // irbuf_print(rb);
  
  snprintf(rb->buf_ptr[rb->write_index],STR_LEN,"%s",inst);
  if(rb->write_index == rb->buf_size - 1){
    rb->write_index = 0;
  }else{
    rb->write_index += 1;
  }
  // Log("after push inst");
  // irbuf_print(rb);
  return ret;
}
void irbuf_print(const struct iringbuf *rb){
  Log("iringbuf for debug(Note:):\n");
  size_t i;
  for(i = 0;i < rb->buf_size;i++){
    if(*(rb->buf_ptr+i)[0]!= '\0'){
      if(i == rb->write_index){
        printf("---->%s\n",*(rb->buf_ptr + i));
      }else{
        printf("     %s\n", *(rb->buf_ptr + i));
      }

    }
    else{
      printf("null inst\n");
    }
  }
}
void iringbuf_init(){
  // TODO: init iringbuf
  irbuf_init(&irbuf, pool,IRING_LEN);
}

void device_update();

static void trace_and_difftest(Decode *_this, vaddr_t dnpc) {
#ifdef CONFIG_ITRACE_COND
// TODO:add inst to iringbuf
  if (ITRACE_COND) { log_write("%s\n", _this->logbuf); irbuf_push(&irbuf, _this->logbuf);}
#endif
  if (g_print_step) { IFDEF(CONFIG_ITRACE, puts(_this->logbuf)); }
  IFDEF(CONFIG_DIFFTEST, difftest_step(_this->pc, dnpc));
  //scan all the watchpoint
  if(!check_wp()){
  nemu_state.state = NEMU_STOP;
  printf("at &pc = %x\n", _this->pc);

  }
  
}

static void exec_once(Decode *s, vaddr_t pc) {
  s->pc = pc;
  s->snpc = pc;
  
  isa_exec_once(s);// will modify s->snpc to next inst's pc
  cpu.pc = s->dnpc;
#ifdef CONFIG_ITRACE
  char *p = s->logbuf;
  p += snprintf(p, sizeof(s->logbuf), FMT_WORD ":", s->pc);
  int ilen = s->snpc - s->pc;
  int i;
  uint8_t *inst = (uint8_t *)&s->isa.inst.val;
  for (i = ilen - 1; i >= 0; i --) {
    p += snprintf(p, 4, " %02x", inst[i]);
  }
  int ilen_max = MUXDEF(CONFIG_ISA_x86, 8, 4);
  int space_len = ilen_max - ilen;
  if (space_len < 0) space_len = 0;
  space_len = space_len * 3 + 1;
  memset(p, ' ', space_len);
  p += space_len;

#ifndef CONFIG_ISA_loongarch32r
  void disassemble(char *str, int size, uint64_t pc, uint8_t *code, int nbyte);
  disassemble(p, s->logbuf + sizeof(s->logbuf) - p,
      MUXDEF(CONFIG_ISA_x86, s->snpc, s->pc), (uint8_t *)&s->isa.inst.val, ilen);
#else
  p[0] = '\0'; // the upstream llvm does not support loongarch32r
#endif
#endif
}

static void execute(uint64_t n) {
  Decode s;


  for (;n > 0; n --) {
    // Log("before exec_once");
    // irbuf_print(&irbuf);
    exec_once(&s, cpu.pc);


    #ifdef CONFIG_FTRACE_COND
      ftrace_print();
    #endif
    g_nr_guest_inst ++;
    // Log("before trace_and_diff");
    // irbuf_print(&irbuf);
    trace_and_difftest(&s, cpu.pc);
    //     Log("after trace_and_diff");
    // irbuf_print(&irbuf);
    if (nemu_state.state != NEMU_RUNNING) break;
    IFDEF(CONFIG_DEVICE, device_update());

    word_t intr = isa_query_intr();
    if(intr != INTR_EMPTY){
      // now intr = 1
      cpu.pc = isa_raise_intr(intr, cpu.pc);
    }
  }
}

static void statistic() {
  IFNDEF(CONFIG_TARGET_AM, setlocale(LC_NUMERIC, ""));
#define NUMBERIC_FMT MUXDEF(CONFIG_TARGET_AM, "%", "%'") PRIu64
  Log("host time spent = " NUMBERIC_FMT " us", g_timer);
  Log("total guest instructions = " NUMBERIC_FMT, g_nr_guest_inst);
  if (g_timer > 0) Log("simulation frequency = " NUMBERIC_FMT " inst/s", g_nr_guest_inst * 1000000 / g_timer);
  else Log("Finish running in less than 1 us and can not calculate the simulation frequency");
}

void assert_fail_msg() {
  isa_reg_display();
  statistic();
}

/* Simulate how the CPU works. */
void cpu_exec(uint64_t n) {
  // iringbuf_init();
  g_print_step = (n < MAX_INST_TO_PRINT);
  switch (nemu_state.state) {
    case NEMU_END: case NEMU_ABORT:
      printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
      return;
    default: nemu_state.state = NEMU_RUNNING;
  }

  uint64_t timer_start = get_time();
// Log("before execute");
// irbuf_print(&irbuf);
  execute(n);

  uint64_t timer_end = get_time();
  g_timer += timer_end - timer_start;


// // test and debug
// irbuf_print(&irbuf);


  switch (nemu_state.state) {
    case NEMU_RUNNING: nemu_state.state = NEMU_STOP; break;

    case NEMU_END: case NEMU_ABORT:
      
      Log("nemu: %s at pc = " FMT_WORD,
          (nemu_state.state == NEMU_ABORT ? ANSI_FMT("ABORT", ANSI_FG_RED) :
           (nemu_state.halt_ret == 0 ? ANSI_FMT("HIT GOOD TRAP", ANSI_FG_GREEN) :
            ANSI_FMT("HIT BAD TRAP", ANSI_FG_RED))),
          nemu_state.halt_pc);
          if(nemu_state.state == NEMU_ABORT){irbuf_print(&irbuf);}

          // #ifdef CONFIG_FTRACE_COND
          // // ftrace_print();
          // #endif
      // fall through
    case NEMU_QUIT: statistic();
  }
  
  #ifdef CONDFIG_FTRACE_COND
    ftrace_free();
  #endif
}
