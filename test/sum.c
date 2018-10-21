 int array[2] = {1,3};

int sum(int n)
{
        int i, s =0;
        for (i=0; i< n; i++)
        {
            static int count = 0 ;
                s+=array[i];
                count++;
                s=count;
        }
        return s;
}
