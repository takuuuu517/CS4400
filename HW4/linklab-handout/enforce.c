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


/* Given the in-memory ELF header pointer as `ehdr` and a section
   header pointer as `shdr`, returns a pointer to the memory that
   contains the in-memory content of the section */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)

Elf64_Shdr *section_by_index(Elf64_Ehdr *ehdr, int index); // from video
Elf64_Shdr *section_by_name(Elf64_Ehdr *ehdr, char* name); // from video
char * op_description(int op);


int mode; // 0 = close, 1 = open
int mode_cp;
// when it sees "close method" add 1
// when it sees "open method" substract 1

 Elf64_Addr last_code_adrr;

void decode_handler(instruction_t ins, code_t *code_ptr, Elf64_Addr code_addr, Elf64_Sym **function_arr, int i, Elf64_Ehdr *ehdr, int mode );



/*************************************************************/

void enforce(Elf64_Ehdr *ehdr) { // ehdr = output so file　header address {e_shoff, e_shstrndx, etc...}
  Elf64_Shdr* shdrs = (void*)ehdr + ehdr->e_shoff;
  char * strs = AT_SEC(ehdr, shdrs + ehdr->e_shstrndx);

  // to print out all section name (.text , .plt, .dynsym)
  int i;
  // Elf64_Shdr * test = section_by_name(ehdr, ".gnu.hash");

  for(i = 0 ; i < ehdr->e_shnum; i++)
  {
    // strs+shdrs[i].sh_name = .text , .plt, .dynsym
    // printf("%s\n", strs+shdrs[i].sh_name);
    printf("%s\n", strs+shdrs[i].sh_name);
    // printf("%p\n", shdrs+i);

    /* testin for section by index ///////////////////////////////////
    // Elf64_Shdr * test = section_by_index(ehdr, i);
    // printf("%s\n", strs+test->sh_name);
    */////////////////////////////////////////////////////////////////

    // /* testin for section by name ///////////////////////////////////
    // printf("%p\n", test);
    // */////////////////////////////////////////////////////////////////
  }
  printf("\n");

  ////////// dynamic symbol //////////////////////////////////
  Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym");
  Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);
  strs = AT_SEC(ehdr, section_by_name(ehdr, ".dynstr"));
  int count = dynsym_shdr->sh_size / sizeof(Elf64_Sym);
  Elf64_Sym **function_arr = malloc(100*sizeof(Elf64_Sym*));

  int f_array_index = 0;
  for(i = 0 ; i< count; i++)
  {
    // printf("%s\n", strs+syms[i].st_name);

    if(ELF64_ST_TYPE(syms[i].st_info) == STT_FUNC && (syms+i)->st_size != 0 )
    {
      function_arr[f_array_index++] = syms+i;
      printf("hello:  %d\t%s\n", (int)(syms+i)->st_size,strs+syms[i].st_name);
      // printf("    %d\n", (syms+i)->st_size );

      // st_size = 0 if size if unknown -> if the function comes from a different file.
      // st_size > 0 if size if determined (e.g. never_ok has size of 21 because never_ok is from .so file)

    }

  }

  printf("\n");
  ////////// dynamic symbol //////////////////////////////////


  ///////// gloval variables //////////////////////////////////////
  Elf64_Shdr *rela_dyn_shdr = section_by_name(ehdr, ".rela.dyn");
  Elf64_Rela *relas = AT_SEC(ehdr, rela_dyn_shdr);
  count = rela_dyn_shdr->sh_size / sizeof(Elf64_Rela);
  for (i = 0; i < count; i++)
  {
    printf("%s\n", strs+syms[ELF64_R_SYM(relas[i].r_info)].st_name);
    // printf("%d\n", (int)ELF64_R_SYM(relas[i].r_info));
    printf("r_offset  %x\n", relas[i].r_offset);
    // printf("\n");f
  }
  printf("\n");
  ///////// gloval variables //////////////////////////////////////


  printf("%s\n","function" );
  ////// gloval function ////////
  Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
  Elf64_Rela *relas2 = AT_SEC(ehdr, rela_plt_shdr);
  count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);
  for (i = 0; i < count; i++)
  {
    printf("%s\n", strs+syms[ELF64_R_SYM(relas2[i].r_info)].st_name);
    printf("r_offset  %x\n", relas2[i].r_offset);
    // printf("%d\n", (int)ELF64_R_SYM(relas2[i].r_info));
    // printf("\n");f
  }
  ////// gloval function ////////

  printf("\n" );

  //function_arr has all the function used in so file (e.g. ex1.so == close_it, open_it, always_ok, etc..)
  // f_array_index is the size of function_arr, sizeof(function_arr) does not work....
  for(i = 0; i < f_array_index; i++)
  {
    if(strcmp(strs+function_arr[i]->st_name, "function_1")==0)
    {
      printf("\n\n\n\n\n\n\nfunction_name:  %s\n", strs+function_arr[i]->st_name);

      int j = function_arr[i]->st_shndx;

      instruction_t ins = {0,0,0};
      code_t* code_ptr = AT_SEC(ehdr, shdrs + j)+(function_arr[i]->st_value - shdrs[j].sh_addr); // from the video 14
      Elf64_Addr code_addr = function_arr[i]->st_value;
      // printf("code_ptr:  %p\n",code_ptr );
      // printf("code_addr: %p\n",code_addr );
      // printf("ins:       %p\n",&ins );


      // typedef struct {
      //   instruction_op_t op; /* operation performed by the instruction */
      //   int length; /* length of the instruction in bytes */
      //   Elf64_Addr addr; /* set only for some kinds of instructions */
      // } instruction_t;
      mode = 0;
      decode_handler(ins, code_ptr, code_addr, function_arr,i, ehdr, mode);
    }


    // decode(&ins, code_ptr, code_addr);
    // printf("ins.op:  %s\n", op_description(ins.op));
    // printf("length:  %d\n", ins.length);
    // printf("code_ptr:  %x\n", *code_ptr);
    // printf("\n" );
    //
    // code_ptr += ins.length;
    // code_addr += ins.length;
    // decode(&ins, code_ptr, code_addr);
    // printf("ins.op:  %s\n", op_description(ins.op));
    // printf("length:  %d\n", ins.length);
    // printf("code_ptr:  %x\n", *code_ptr);
    // printf("\n" );


    // int accumBytes;
    // int k;
    // for(accumBytes = 0; accumBytes < function_arr[i]->st_size; accumBytes+=ins.length)
    // {
    //   decode(&ins, code_ptr, code_addr);
    //   for(k = 0; k<ins.length; k++)
    //     printf("%x ", code_ptr[k]);
    //
    //     // code_ptr += ins.length;
    //     code_ptr = (char*)code_ptr+ins.length;
    //
    //   // code_addr += ins.length;
    //
    //   printf("\n" );
    // }


  }


}

void decode_handler(instruction_t ins, code_t *code_ptr, Elf64_Addr code_addr, Elf64_Sym **function_arr, int i ,Elf64_Ehdr *ehdr, int mode)
{
  // int mode = 0;

  int accumBytes;
  for(accumBytes = 0; accumBytes < function_arr[i]->st_size; accumBytes+=ins.length) // go to each instruction in the function
  {
    decode(&ins, code_ptr, code_addr);
    printf("ins.op:  %s\n", op_description(ins.op)) ;
    printf("length:  %d\n", ins.length);
    printf("code_ptr:  %x\n", *code_ptr);
    printf("ins.addr:  %p\n", ins.addr);
    printf("code_addr:  %p\n", code_addr);

    if(  ins.op == MOV_ADDR_TO_REG_OP)
    {
      Elf64_Shdr* shdrs = (void*)ehdr + ehdr->e_shoff;
      char *strs = AT_SEC(ehdr, section_by_name(ehdr, ".dynstr"));
      Elf64_Shdr *rela_dyn_shdr = section_by_name(ehdr, ".rela.dyn");
      Elf64_Rela *relas = AT_SEC(ehdr, rela_dyn_shdr);
      Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym");
      Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);

      char* variable;

      int k, count = rela_dyn_shdr->sh_size / sizeof(Elf64_Rela);

      for (k = 0; k < count; k++) // determine  if there are gloval variable.
      {
        if(relas[k].r_offset == ins.addr) // gloval variable found.
        {
          variable = strs+syms[ELF64_R_SYM(relas[k].r_info)].st_name;
          if(mode == 0 && is_protected(variable)) // if close mode and protect_variable -> replace with crash
          {
            replace_with_crash(code_ptr, &ins);
          }
          printf("%s\n",variable);
        }
      }
      last_code_adrr = code_addr;
      printf("%s\n", "mov");
    }
    else if( ins.op == JMP_TO_ADDR_OP)
    {
      last_code_adrr = code_addr;
      printf("%s\n", "jmp");
    }
    else if(  ins.op == MAYBE_JMP_TO_ADDR_OP)
    {
      //if
      // int offset = ins.addr - code_addr;
      // decode_handler(ins,code_ptr+offset, ins.addr, function_arr,  i, ehdr);

      // else
      // code_ptr += ins.length;
      // code_addr += ins.length;
      // printf("else  \n" );
      // decode_handler(ins,code_ptr, ins.addr, function_arr,  i, ehdr);



      // if
      printf("else  \n" );

      // decode(&ins3, code_ptr+ins.length, code_addr+ins.length);
      // printf("asdf:   %x\n",*code_ptr );
      // printf("asdf:   %x\n",*(code_ptr+ins.length) );

      // mode_cp = mode;
      decode_handler(ins,code_ptr+ins.length, code_addr+ins.length, function_arr,  i, ehdr, mode);
      // if(mode != 0 )
      // replace_with_crash( code_ptr+ins.length, &ins3);


      printf("\nasdkfhaksdhflkashd;fjas;ldf;lasdjf;ljas;lkdjf           aslfja;lsfalksj\n\n" );

      // else
      // instruction_t ins3 = {0,0,0};

      int offset = ins.addr - code_addr;
      instruction_t ins2 = {0,0,0};
      // decode(&ins3, code_ptr+offset, ins.addr);
      // mode =mode_cp;

      decode_handler(ins2,  code_ptr+offset,  ins.addr, function_arr,  i, ehdr ,mode);


      Elf64_Addr special_length = last_code_adrr - code_addr;
      code_ptr += special_length;
      code_addr += special_length;
      accumBytes+= special_length;
      printf("%x\n", code_addr);



      // if(mode != 0 )
      // replace_with_crash( code_ptr+offset, &ins3);
      last_code_adrr = code_addr;

      printf("%s\n", "maybe jmp");
    }
    else if( ins.op == CALL_OP ) // call open and close, etc
    {
      char *strs = AT_SEC(ehdr, section_by_name(ehdr, ".dynstr"));
      Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
      Elf64_Rela *relas2 = AT_SEC(ehdr, rela_plt_shdr);
      int k,count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);
      Elf64_Shdr *dynsym_shdr = section_by_name(ehdr, ".dynsym");
      Elf64_Sym *syms = AT_SEC(ehdr, dynsym_shdr);

      instruction_t plt_ins = {0,0,0};
      Elf64_Addr plt_addr = ins.addr;
      int offset = plt_addr - code_addr;
      code_t *plt_ptr = code_ptr + offset;
      /* printf("\t%p\n", ins.jmp_to_addr.addr); */

      printf("plt\n" );
      decode(&plt_ins, plt_ptr, plt_addr);
      printf("plt_ins.op:  %s\n", op_description(plt_ins.op)) ;
      printf("length:  %d\n", plt_ins.length);
      printf("plt_ptr:  %x\n", *plt_ptr);
      printf("plt_ins.addr:  %p\n", plt_ins.addr);


      for (k = 0; k < count; k++)
      {
        if(relas2[k].r_offset == plt_ins.addr)
        {
          if(strcmp(strs+syms[ELF64_R_SYM(relas2[k].r_info)].st_name, "open_it")==0)
          {
            mode++;
            printf("%s\n","open" );
          }
          else if(strcmp(strs+syms[ELF64_R_SYM(relas2[k].r_info)].st_name, "close_it")==0)
          {
            mode--;
            printf("%s\n","close" );
          }
          printf("mode  %d\n", mode );
          if(!(mode == 0 || mode == 1))
          {
            replace_with_crash(code_ptr, &ins);
          }
        }

        // printf("%s\n", strs+syms[ELF64_R_SYM(relas2[k].r_info)].st_name);
        // printf("r_offset  %x\n", relas2[k].r_offset);
        // printf("%d\n", (int)ELF64_R_SYM(relas2[i].r_info));
        // printf("\n");f
      }





      // int count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);

      last_code_adrr = code_addr;
      printf("%s\n", "call");
    }
    // else if(strcmp(op_description(ins.op), "RET_OP") == 0)
    else if( ins.op == RET_OP)
    {
      if(mode != 0 )
      {
        printf("%x\n", code_ptr );
        replace_with_crash(code_ptr, &ins);
      }

      printf("mode at return:  %d\n", mode);
      printf("%s\n", "return ");
      last_code_adrr = code_addr;
      return;
    }
    else if( ins.op == OTHER_OP)
    {
      // can be ignored
      last_code_adrr = code_addr;
      printf("%s\n", "other");
    }
    else
    {
      printf("%s\n", "NO");
    }


    printf("\n" );

    code_ptr += ins.length;
    code_addr += ins.length;
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
    return "error";
}


Elf64_Shdr *section_by_index(Elf64_Ehdr *ehdr, int index)
{
  Elf64_Shdr * result ;
  Elf64_Shdr* shdrs = (void*)ehdr + ehdr->e_shoff;
  result = shdrs+index;
  return result;
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
    {
      result = shdrs + i;
    }
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



  // printf("%s\n",argv[0] ); // ./enforce.
  // printf("%s\n",argv[1] ); // ex0.so
  // printf("%s\n",argv[2] ); // dest.so

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
