#include <stdio.h>

void greet(char*);

int main(int argc, char* argv[])
{
  int i = 1;
  while(i != argc)
    {
      greet(argv[i]);
      i++;
    }
  

  return 0;
}


void greet(char* name)
{
  printf("hello %s\n" ,name); 
}
