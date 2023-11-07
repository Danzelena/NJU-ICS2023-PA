#include "elfloader.h"

FILE *file;
char *strtab;
Elf32_Sym *symtab;
Elf32_Shdr *sym = NULL;

// TODO:func to load and parse ELF file
void init_ftrace(const char *fn)
{
//   char buff[255];
  if (fn != NULL)
  {
    FILE *fp = fopen(fn, "rb");
    // Assert(fp, "Can't open '%s'", fn);
    file = fp;
  }
  if (file)
  {
    // read the header
    Elf32_Ehdr header;
    size_t n = fread(&header, sizeof(header), 1, file);
    if(n == 0){printf("Error!\n");}
    // check elf
    if (memcmp(header.e_ident, ELFMAG, SELFMAG) == 0)
    {
      // valid elf
      // section table
      Elf32_Off sec_off = header.e_shoff;
    //   uint16_t sec_num = header.e_shnum;
      // printf("sec_off = 0x%lx, src_num = %u\n", (unsigned long)sec_off, (unsigned)sec_num);

      // Assert(sec_off != 0,"may have section table\n");

      fseek(file, sec_off, SEEK_SET);

      // get sym and str table
      Elf32_Shdr *section = malloc(header.e_shentsize * header.e_shnum);
      n = fread(section, header.e_shentsize, header.e_shnum, file);
      Elf32_Shdr *str = NULL;
      for (uint16_t i = 0; i < header.e_shnum; i++)
      {
        // get the name of section
        char *name_addr = (char *)((uintptr_t)section[header.e_shstrndx].sh_offset + section[i].sh_name);
        fseek(file, (long)name_addr, SEEK_SET);
        char *name = (char *)malloc(30);
        n = fread(name, 30 * sizeof(char), 1, file);
        printf("Debug: section name = %s\n", name);

        if (strcmp(name, ".symtab") == 0)
        {
          sym = &section[i];
        }
        else if (strcmp(name, ".strtab") == 0)
        {
          str = &section[i];
        }
      }

      // read string table
      fseek(file, str->sh_offset, SEEK_SET);
      strtab = malloc(str->sh_size);
      n = fread(strtab, str->sh_size, 1, file);

      // read symbol table
      fseek(file, sym->sh_offset, SEEK_SET);
      symtab = malloc(sym->sh_size);
      n = fread(symtab, sym->sh_size, 1, file);
        
    //   for (uint16_t i = 0; i < sym->sh_size / sizeof(Elf32_Sym); i++)
    //   {
    //     Elf32_Sym symbol = symtab[i];
    //     char *symbol_name = &strtab[symbol.st_name];
    //     // printf("Debug:Symbol %d:%s\n", i, symbol_name);
    //   }
    }

    // close file
    fclose(file);
  }
}
// check addr in [Value, Value + Size),
// if so, return according function name
char *gen_func_name(uint32_t addr)
{
  for (uint16_t i = 0; i < sym->sh_size / sizeof(Elf32_Sym); i++)
  {
    Elf32_Sym symbol = symtab[i];
    if (ELF32_ST_TYPE(symbol.st_info) == STT_FUNC)
    {

      char *symbol_name = &strtab[symbol.st_name];
      // printf("Debug:Symbol %d:%s\n", i, symbol_name);

      Elf32_Addr Value = symbol.st_value;
      uint32_t Size = symbol.st_size;
      if (Value <= addr && addr < Value + Size)
      {
        printf("symbol name = %s\n", symbol_name);
        return symbol_name;
      }
    }
  }
  return "???";
}
// int main()
// {

//   char file_name[] = "a_plus_b";
//   init_ftrace(file_name);
//   uint32_t addr_test = 0x116a;
//   printf("%s\n",gen_func_name(addr_test));
// }