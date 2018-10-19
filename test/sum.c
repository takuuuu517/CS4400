extern int array[];

int sum(int n)
{
        int i, s =0;
        for (i=0; i< n; i++)
        {
                s+=array[i];
        }
        return s;
}
