#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

// int sum(int n);

// static int array[2] = {1,2};

int is_protected(char * v)
{

  int len = strlen(v);
  int i;
  char * sub = malloc(15*sizeof(char));

  if(len < 10){

    return 0;
  }
  else
  {
    for(i=0; i< 10; i++)
    {
      sub[i] = v[i];
    }
    if(strcmp(sub, "protected_") == 0)
      return 1;
  }
  return 0;
}

int main()
{
    char* v = "protected";
    printf("%d\n",is_protected(v) );

}
