#include <stdio.h>

int main()
{
  int arr[20][30];
  int *p = &arr[3][29];

  arr[4][0]=1;
  arr[4][1]=2;
  arr[4][2]=3;
  arr[4][3]=4;

  // printf("%d",*(p + 4));
  printf("%d\n", sizeof(int));

  //  printf("hello world\n");
  return 0;
}
