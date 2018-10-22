#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dlfcn.h>

static char *make_relative(char *p);

int main(int argc, char **argv) {
  char *path;
  void *so;
  int (*f)(int);
  int r;

  if (argc != 4) {
    fprintf(stderr,
	    "%s: expects three arguments: <file.so> <function> <int>\n",
	    argv[0]);
    return 1;
  }

  // printf("%s\n", argv[0]);   ./call
  // printf("%s\n", argv[1]);   .so file
  // printf("%s\n", argv[2]);   method used in the .so file
  // printf("%s\n\n", argv[3]); parameter of the method

  path = argv[1]; // ./call
  if (path[0] != '/')
    path = make_relative(path);
  so = dlopen(path, RTLD_NOW | RTLD_GLOBAL);
  if (!so) {
    fprintf(stderr, "error opening %s\n", path);
    return 1;
  }

  f = dlsym(so, argv[2]); // f = method in the so file
  if (!f) {
    fprintf(stderr, "could not find %s in %s\n", argv[2], argv[1]);
    return 1;
  }

  r = f(atoi(argv[3])); // f = ex. always_ok method

  printf("%d\n", r);

  return 0;
}

static char *make_relative(char *p)
{
  int len = strlen(p);
  char *p2 = malloc(len + 3);
  p2[0] = '.';
  p2[1] = '/';
  strcpy(p2+2, p);
  return p2;
}
