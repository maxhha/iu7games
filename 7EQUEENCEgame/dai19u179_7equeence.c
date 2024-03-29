#include <stdio.h>

#define N 1000
#define M 13

long long sequence_game(int array[])
{
    long long r = 1, max = 0;
    int i = 0, j = 0;
    while(i < N)
    {
        if (array[i] == 0)
        {
            if (i >= M + j && r > max)
            {
                max = r;
            }
            j = ++i;
            r = 1;
        }
        else if (i < M + j)
        {
            r *= array[i++];
        }
        else
        {
            if (r > max)
            {
                max = r;
            }
            r /= array[j++];
            r *= array[i++];
        }
    }
    return max;
}
