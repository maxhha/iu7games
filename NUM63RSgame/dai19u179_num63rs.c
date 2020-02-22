#include <stdio.h>
#define COUNTS_N 1000000

int numbers_game(int min, int max)
{
    int counts[COUNTS_N];
    
    for(int i = 0; i < COUNTS_N; i++)
        counts[i] = 0;
     
    int c;
    for(int i = min; i <= max; i++) {
        c = i;
        int m = 0, j;
        for(j = 2; j <= c;) {
            if (c % j == 0) {
                m += 1;
                c /= j;
            } else {
                if (counts[j - 2] < m) {
                    counts[j - 2] = m;
                }
                m = 0;
                j++;
            }
        }
        if (counts[j - 2] < m) {
            counts[j - 2] = m;
        }
    }
    int r = 1;
    
    for(int i = 0; i < COUNTS_N;)
    {
        if (counts[i] == 0) {
            i++;
        } else {
            r *= i + 2;
            counts[i] -= 1;
        }
    }
    return r;
}
