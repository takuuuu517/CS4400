#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <elf.h>
#include "decode.h"

/*
 * Taku Sakikawa
 * U1075644
 * CS440 - Fall 2018
 */

/* Given the in-memory ELF header pointer as `ehdr` and a section
   header pointer as `shdr`, returns a pointer to the memory that
   contains the in-memory content of the section */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)

Elf64_Shdr *section_by_name(Elf64_Ehdr *ehdr, char* name); // from video
int is_protected(char * v);
char * op_description(int op);
void decode_handler(instruction_t ins, code_t *code_ptr, Elf64_Addr code_addr, Elf64_Sym **function_arr, int i, Elf64_Ehdr *ehdr, int mode );

int mode; // 0 = close, 1 = open
// when it sees "close method" add 1
// when it sees "open method" substract 1

 Elf64_Addr last_code_adrr;
/*************************************************************/

void enforce(Elf64_Ehdr *ehdr) { // ehdr = output so file　header address {e_shoff, e_shstrndx, etc...}
  Elf64_Shdr* shdrs = (void*)ehdr + ehdr->e_shoff;
  int i;
  Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym");
  Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);
  int count = dynsym_shdr->sh_size / sizeof(Elf64_Sym);
  Elf64_Sym **function_arr = malloc(100*sizeof(Elf64_Sym*));
  int f_array_index = 0;

  for(i = 0 ; i< count;  i++) // put all the functions declared in the so file to an array
  {
    if(ELF64_ST_TYPE(syms[i].st_info) == STT_FUNC && (syms+i)->st_size != 0 )
      function_arr[f_array_index++] = syms+i; // create an array that contains pointer to the address of each function in the so file
  }

  Elf64_Shdr *rela_dyn_shdr = section_by_name(ehdr, ".rela.dyn");
  count = rela_dyn_shdr->sh_size / sizeof(Elf64_Rela);

  Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
  count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);

  //iterate through the functions
  for(i = 0; i < f_array_index; i++) //f_array_index is how many functions there are in the array
  {
    // if(strcmp(strs+function_arr[i]->st_name, "function_1")==0) // To test one function in the file
    // {
      int j = function_arr[i]->st_shndx;

      instruction_t ins = {0,0,0};
      code_t* code_ptr = AT_SEC(ehdr, shdrs + j)+(function_arr[i]->st_value - shdrs[j].sh_addr); // from the video 14
      Elf64_Addr code_addr = function_arr[i]->st_value;

      mode = 0;
      decode_handler(ins, code_ptr, code_addr, function_arr,i, ehdr, mode); // decode each function.
    // }
  }
}

void decode_handler(instruction_t ins, code_t *code_ptr, Elf64_Addr code_addr, Elf64_Sym **function_arr, int i ,Elf64_Ehdr *ehdr, int mode)
{
  char *strs = AT_SEC(ehdr, section_by_name(ehdr, ".dynstr"));
  Elf64_Shdr *rela_dyn_shdr = section_by_name(ehdr, ".rela.dyn");
  Elf64_Rela *relas = AT_SEC(ehdr, rela_dyn_shdr);
  Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym" );
  Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);
  Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
  Elf64_Rela *relas2 = AT_SEC(ehdr, rela_plt_shdr);
  int k,count;

  int accumBytes;
  for(accumBytes = 0; accumBytes < function_arr[i]->st_size; accumBytes+=ins.length) // go to each instruction in the function
  {
    decode(&ins, code_ptr, code_addr); // decode each instruction in the funciton.

    if(  ins.op == MOV_ADDR_TO_REG_OP)
    {
      char* variable;
      int count = rela_dyn_shdr->sh_size / sizeof(Elf64_Rela);

      for (k = 0; k < count; k++) // determine  if there are gloval variable.
      {
        if(relas[k].r_offset == ins.addr) // gloval variable found.
        {
          variable = strs+syms[ELF64_R_SYM(relas[k].r_info)].st_name;
          if(mode == 0 && is_protected(variable)) // if close mode and protect_variable -> replace with crash
            replace_with_crash(code_ptr, &ins);
        }
      }
      last_code_adrr = code_addr;

      code_ptr += ins.length;
      code_addr += ins.length;
    }
    else if( ins.op == JMP_TO_ADDR_OP) // not sure the purpose of this
    {
      last_code_adrr = code_addr;
      return;
    }
    else if(  ins.op == MAYBE_JMP_TO_ADDR_OP)
    {
      /*
       * decode if and else part of the instructions separately
       * in assembly code else instruction comes before if
       * code_addr+ins.length is the next instruction, which is the start of the instruction of else
       * ins.addr has the run time address of the start address of if instruction
       * ins.addr - code_addr = (offset) is how far the start of the if is from the current address
       * adding the offset to the current addrest (code_addr) is the start address of the if instruction
       * code_addr+ (last_code_adrr - code_addr)
       */

      // else part
      decode_handler(ins,code_ptr+ins.length, code_addr+ins.length, function_arr,  i, ehdr, mode);

      int offset = ins.addr - code_addr; // calculate the offset to the if part
      instruction_t ins2 = {0,0,0};
      // decode if part
      decode_handler(ins2,  code_ptr+offset,  ins.addr, function_arr,  i, ehdr ,mode);

      // special_length is the offset to the last instruction of the if (return)
      Elf64_Addr special_length = last_code_adrr - code_addr;
      code_ptr += special_length;
      code_addr += special_length;
      accumBytes+= special_length;

      last_code_adrr = code_addr;
    }
    else if( ins.op == CALL_OP ) // call open and close, etc
    {
      instruction_t called_ins = {0,0,0};
      Elf64_Addr called_addr = ins.addr;
      int offset = called_addr - code_addr;
      code_t *called_ptr = code_ptr + offset;

      decode(&called_ins, called_ptr, called_addr); // decode the first instruction of the called function

      count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);
      for (k = 0; k < count; k++)
      {
        if(relas2[k].r_offset == called_ins.addr)
        {
          if(strcmp(strs+syms[ELF64_R_SYM(relas2[k].r_info)].st_name, "open_it")==0)
            mode++;
          else if(strcmp(strs+syms[ELF64_R_SYM(relas2[k].r_info)].st_name, "close_it")==0)
            mode--;
          if(!(mode == 0 || mode == 1))
            replace_with_crash(code_ptr, &ins);
        }
      }

      last_code_adrr = code_addr;
      code_ptr += ins.length;
      code_addr += ins.length;
    }
    else if( ins.op == RET_OP)
    {
      if(mode != 0 )
        replace_with_crash(code_ptr, &ins);
      return;
    }
    else if( ins.op == OTHER_OP)
    {
      // can be ignored
      last_code_adrr = code_addr;
      code_ptr += ins.length;
      code_addr += ins.length;
    }
  }
}

/*************************************************************/

/*
 * determine whether the variable is protected
 * return 1 if it is protected
 * return 0 if it is not.
 */
int is_protected(char * v)
{
  int len = strlen(v);
  int i;
  char * sub = malloc(15*sizeof(char));

  if(len < 10)
    return 0;
  else
  {
    for(i=0; i< 10; i++)
      sub[i] = v[i];
    if(strcmp(sub, "protected_") == 0)
      return 1;
  }
  return 0;
}

// for testing
char * op_description(int op)
{
  if(op == 0)
    return "MOV_ADDR_TO_REG_OP";
  else if( op== 1)
    return "JMP_TO_ADDR_OP";
  else if( op== 2)
    return "MAYBE_JMP_TO_ADDR_OP";
  else if( op== 3)
    return "CALL_OP";
  else if( op== 4)
    return "RET_OP";
  else if( op== 5)
    return "OTHER_OP";
  else
    return "ERROR";
}

Elf64_Shdr *section_by_name(Elf64_Ehdr *ehdr, char* name)
{
  Elf64_Shdr * result = NULL;
  Elf64_Shdr* shdrs = (void*)ehdr + ehdr->e_shoff;
  char * strs = AT_SEC(ehdr, shdrs + ehdr->e_shstrndx);
  int i;

  for(i = 0 ; i < ehdr->e_shnum; i++)
  {
    if(strcmp(name,strs+shdrs[i].sh_name)==0)
      result = shdrs + i;
  }
  return result;
}

static void fail(char *reason, int err_code) {
  fprintf(stderr, "%s (%d)\n", reason, err_code);
  exit(1);
}

static void check_for_shared_object(Elf64_Ehdr *ehdr) {
  if ((ehdr->e_ident[EI_MAG0] != ELFMAG0)
      || (ehdr->e_ident[EI_MAG1] != ELFMAG1)
      || (ehdr->e_ident[EI_MAG2] != ELFMAG2)
      || (ehdr->e_ident[EI_MAG3] != ELFMAG3))
    fail("not an ELF file", 0);

  if (ehdr->e_ident[EI_CLASS] != ELFCLASS64)
    fail("not a 64-bit ELF file", 0);

  if (ehdr->e_type != ET_DYN)
    fail("not a shared-object file", 0);
}

int main(int argc, char **argv) {
  int fd_in, fd_out;
  size_t len;
  void *p_in, *p_out;
  Elf64_Ehdr *ehdr;

  if (argc != 3)
    fail("expected two file names on the strcmpmand line", 0);

  /* Open the shared-library input file */
  // int open(const char *path, int oflags);
  fd_in = open(argv[1], O_RDONLY); // Open the file so that it is read only.
  if (fd_in == -1)
    fail("could not open input file", errno);

  /* Find out how big the input file is: */
  // off_t lseek(int fildes, off_t offset, int whence);
  len = lseek(fd_in, 0, SEEK_END); // 	Offset is to be measured relative to the end of the file.

  /* Map the input file into memory: */
  p_in = mmap(NULL, len, PROT_READ, MAP_PRIVATE, fd_in, 0);
  if (p_in == (void*)-1)
    fail("mmap input failed", errno);

  /* Since the ELF file starts with an ELF header, the in-memory image
     can be cast to a `Elf64_Ehdr *` to inspect it: */
  ehdr = (Elf64_Ehdr *)p_in;

  /* Check that we have the right kind of file: */
  check_for_shared_object(ehdr);

  /* Open the shared-library output file and set its size: */
  fd_out = open(argv[2], O_RDWR | O_CREAT, 0777);
  if (fd_out == -1)
    fail("could not open output file", errno);
  if (ftruncate(fd_out, len))
    fail("could not set output file file", errno);

  /* Map the output file into memory: */
  p_out = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_out, 0);
  if (p_out == (void*)-1)
    fail("mmap output failed", errno);

  /* Copy input file to output file: */
  memcpy(p_out, p_in, len);

  /* Close input */
  if (munmap(p_in, len))
    fail("munmap input failed", errno);
  if (close(fd_in))
    fail("close input failed", errno);

  ehdr = (Elf64_Ehdr *)p_out;

  enforce(ehdr);

  if (munmap(p_out, len))
    fail("munmap input failed", errno);
  if (close(fd_out))
    fail("close input failed", errno);

  return 0;
}
