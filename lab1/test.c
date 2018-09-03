#include <stdio.h>

void greet (char*);

int main (int argc, char* argv[])
{

  printf("type: %s\n",typeid(argv[1]).name());

}

