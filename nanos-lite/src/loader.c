#include <proc.h>
#include <elf.h>
#include "../include/fs.h"

#ifdef __LP64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Off Elf64_Off
#define Elf_Addr Elf64_Addr
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Off Elf32_Off
#define Elf_Addr Elf32_Addr
#endif

// FORWARD: finish this define match ststem
// #if defined(__ISA_AM_NATIVE__)
// #define EXPECT_TYPE EM_X86_64
// #elif defined(__ISA_X86__)
// #define EXPECT_TYPE EM_X86_64
// #elif defined(__ISA_RISCV_H__)
// #define EXPECT_TYPE EM_RISCV
// #else
// # error Unsupported ISA
// #endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);



uintptr_t elf_loader(uintptr_t file_off, bool *suc);
uintptr_t elf_file_loader(int fd, bool *suc);
static uintptr_t loader(PCB *pcb, const char *filename)
{
  printf("begin to load %s!\n",filename);
  bool suc = false;
  uintptr_t proaddr = 0;
  // 使用 文件系统 之后的 loader
  int file_fd = fs_open(filename, 0, 0);
  if(file_fd  < 0 || filename == NULL){
    printf("fail to load file\n");
    return -1;
  }
  printf("0\n");
  proaddr = elf_file_loader(file_fd, &suc);

  // 之前的 loader, 从ramdisk中打开elf
  // uintptr_t file_off = 0;
  // proaddr = elf_loader(file_off, &suc);

  if (!(suc))
  {
    panic("Sorry! Can't load ELF file\n");
    assert(0);
  }
  printf("finish load!\n");
  fs_close(file_fd);
  // TODO();
  // printf("proaddr=%x\n",proaddr);
  return proaddr;
}
uintptr_t entry_get(PCB *pcb, const char *filename){
  uintptr_t entry = loader(pcb, filename);
  assert(entry != -1);
  return entry;
}
void naive_uload(PCB *pcb, const char *filename)
{
  uintptr_t entry = loader(pcb, filename);
  if(entry == -1){
    return ;
  }
  Log("Jump to entry = %p", entry);
  ((void (*)())entry)();
}

uintptr_t elf_file_loader(int fd, bool *suc)
{
  printf("1\n");
  Elf_Ehdr header;
  size_t flag = fs_read(fd, &header, sizeof(Elf_Ehdr));
  if (flag <= 0)
  {
    panic("Sorry! read elf header from ramdisk fail!\n");
    *suc = false;
  }
  // check magic number
  if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0)
  {
    panic("Sorry! magic number of 'elf'(maybe) file is invalid!\n");
    assert(0);
  }
  printf("2\n");
  Elf_Off program_off = header.e_phoff;
  size_t program_cnt;
  for (program_cnt = 0; program_cnt < header.e_phnum; program_cnt++)
  {
    Elf_Phdr *program = malloc(header.e_phentsize);
    int flag = fs_lseek(fd, program_off + program_cnt * header.e_phentsize, 0);
    if (flag < 0)
    {
      panic("Sorry! lseek in loader error!\n");
    }
    flag = fs_read(fd, program, header.e_phentsize);
    if (flag <= 0)
    {
      panic("Sorry! fs_read in loader error!\n");
    }
    if (program->p_type == PT_LOAD)
    {
      // load this segementation

      Elf_Off Offset = program->p_offset;
      Elf_Addr VirtAddr = program->p_vaddr;

      // printf("Debug:program%d:O=0x%x,V=0x%x\n", program_cnt, Offset, VirtAddr);

      uint64_t FileSiz = program->p_filesz;
      uint64_t MemSiz = program->p_memsz;
      // printf("Debug:F=0x%x,M=0x%x\n", FileSiz, MemSiz);

      // [VirtAddr,VirtAddr + MemSiz]<- Ramdisk[0 + Program Table Offset + Segment offset]
      flag = fs_lseek(fd, Offset, 0);
      if (flag < 0)
      {
        panic("Sorry! lseek in loader error!\n");
      }
      fs_read(fd, (char *)VirtAddr, MemSiz);
      // ramdisk_read((char *)VirtAddr, file_off + Offset, MemSiz);
      // memcpy((char*)VirtAddr,program,MemSiz);
      memset((char *)VirtAddr + FileSiz, 0, MemSiz - FileSiz);
    }
  }
  printf("3\n");
  *suc = true;
  return header.e_entry;
}
// elf_handle function
uintptr_t elf_loader(uintptr_t file_off, bool *suc)
{

  Elf_Ehdr header;
  size_t flag = ramdisk_read(&header, file_off, sizeof(Elf_Ehdr));
  if (flag == 0)
  {
    panic("Sorry! read elf header from ramdisk fail!\n");
    *suc = false;
  }
  // check magic number
  if (memcmp(header.e_ident, ELFMAG, SELFMAG) != 0)
  {
    panic("Sorry! magic number of 'elf'(maybe) file is invalid!\n");
    assert(0);
  }

  // check ISA type

  // if (header.e_machine != EXPECT_TYPE)
  // {
  //   panic("Sorry! The expected architecture of elf file mismatch the os!\n");
  //   assert(0);
  // }

  Elf_Off program_off = header.e_phoff;
  // BUG:may have bug because malloc of klib is not implement!'
  // printf("Debug:program_off=0x%x\n", program_off);
  size_t program_cnt;
  for (program_cnt = 0; program_cnt < header.e_phnum; program_cnt++)
  {
    // 每次读取一个segenment
    Elf_Phdr *program = malloc(header.e_phentsize);
    flag = ramdisk_read(program, file_off + program_off + program_cnt * header.e_phentsize, header.e_phentsize);

    // check PT_LOAD
    if (program->p_type == PT_LOAD)
    {
      // load this segementation

      Elf_Off Offset = program->p_offset;
      Elf_Addr VirtAddr = program->p_vaddr;

      printf("Debug:program%d:O=0x%x,V=0x%x\n", program_cnt, Offset, VirtAddr);

      uint64_t FileSiz = program->p_filesz;
      uint64_t MemSiz = program->p_memsz;
      printf("Debug:F=0x%x,M=0x%x\n", FileSiz, MemSiz);

      // [VirtAddr,VirtAddr + MemSiz]<- Ramdisk[0 + Program Table Offset + Segment offset]
      ramdisk_read((char *)VirtAddr, file_off + Offset, MemSiz);
      // memcpy((char*)VirtAddr,program,MemSiz);
      memset((char *)VirtAddr + FileSiz, 0, MemSiz - FileSiz);
    }
  }
  *suc = true;
  return header.e_entry;
}

// 找到program header table<=>(Program Table offset)
// 对每一个表项,(指定一个segment,)<=> (Segment offset)
//  if Type == PT_LOAD

//    加载到内存中:
// offset
// VirtAddr
// FileSiz
// MemSiz

// [VirtAddr,VirtAddr + MemSiz]<- Ramdisk[0 + Program Table Offset + Segment offset]

//  [VirtAddr + FileSiz ,VirtAddr + MemSiz]<- 0
