// #include <stdio.h>


extern short ns[6];
int w() {
  printf("%s\n","asdf" );
  printf("%d\n",ns[0] );
  printf("%d\n",ns[1] );
  printf("%d\n",ns[2] );
  printf("%d\n",ns[3] );
  printf("%d\n",ns[4] );
  printf("%d\n",ns[5] );

  return ns[1] + ns[2];
}
