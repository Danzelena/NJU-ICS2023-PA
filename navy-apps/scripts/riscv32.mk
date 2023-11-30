include $(NAVY_HOME)/scripts/riscv/common.mk
CFLAGS  += -march=rv32g -mabi=ilp32  #overwrite
# 用户程序需要被链接到内存位置0x83000000(riscv32)附近
LDFLAGS += -melf32lriscv
