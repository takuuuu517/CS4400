void remove_element(int *array, int index, int array_length)
{
   int i;
   for(i = index; i < array_length - 1; i++)
    array[i] = array[i + 1];
}

void remove_row(int array[][2], int row, int row_n, int col_n )
{
  int i,j;
  for(i = row; i< row_n; i++)
    for(j = 0 ; j< col_n; j++)
      array[i][j] = array[i+1][j];
}

int main(int argc, char const *argv[]) {

  // int arr[5] = {0,1,2,3,4};
  // remove_element(arr, 2, 5);
  //
  // int i ;
  // for(i=0;i<4;i++)
  // {
  //   printf("%d\n",arr[i] );
  // }

  int arr[3][2] = {{1,2},{3,4},{5,6}};
  remove_row(arr, 1, 3,2);

  int i,j ;
  for(i=0;i<2;i++)
  {
    for(j=0;j<2;j++)
      printf("%d ",arr[i][j] );

    printf("\n" );
  }

  /* code */
  return 0;
}
