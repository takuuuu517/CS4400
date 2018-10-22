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

/* Given the in-memory ELF header pointer as `ehdr` and a section
   header pointer as `shdr`, returns a pointer to the memory that
   contains the in-memory content of the section */
#define AT_SEC(ehdr, shdr) ((void *)(ehdr) + (shdr)->sh_offset)

Elf64_Shdr *section_by_index(Elf64_Ehdr *ehdr, int index); // from video
Elf64_Shdr *section_by_name(Elf64_Ehdr *ehdr, char* name); // from video



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

    if(ELF64_ST_TYPE(syms[i].st_info) == STT_FUNC)
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
    printf("%d\n", (int)ELF64_R_SYM(relas[i].r_info));
    // printf("\n");f
  }
  printf("\n");
  ///////// gloval variables //////////////////////////////////////



  ////// gloval function ////////
  Elf64_Shdr *rela_plt_shdr = section_by_name(ehdr, ".rela.plt");
  Elf64_Rela *relas2 = AT_SEC(ehdr, rela_plt_shdr);
  count = rela_plt_shdr->sh_size / sizeof(Elf64_Rela);
  for (i = 0; i < count; i++)
  {
    printf("%s\n", strs+syms[ELF64_R_SYM(relas2[i].r_info)].st_name);
    printf("%d\n", (int)ELF64_R_SYM(relas2[i].r_info));
    // printf("\n");f
  }
  ////// gloval function ////////

  printf("\n" );

  //function_arr has all the function used in so file (e.g. ex1.so == close_it, open_it, always_ok, etc..)
  // f_array_index is the size of function_arr, sizeof(function_arr) does not work....
  for(i = 0; i < f_array_index; i++)
  {
    printf("function_name:  %s\n", strs+function_arr[i]->st_name);


  }






}

/*************************************************************/

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
    fail("expected two file names on the command line", 0);



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
