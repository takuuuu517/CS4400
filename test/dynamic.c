#include <stdio.h>
#include <stdlib.h>
// #include <math.h>
#include <dlfcn.h>

double (*cos_ptr)(double);

int main(int argc, char ** argv)
{
  void* libm = dlopen("libm.so", RTLD_NOW);
  cos_ptr = dlsym(libm,"cos");
  printf("%f\n",cos_ptr(atof(argv[1])) );
  return 0;

}
