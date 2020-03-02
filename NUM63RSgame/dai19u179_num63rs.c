#include <stdio.h>
#define COUNTS_N 100

int numbers_game(int min, int max)
{
    int counts[COUNTS_N] = {0};
    int simples[COUNTS_N] = {2, 3, 5, 7, 11, 13, 17, 19, 23};
    int simples_n = 9;

    if (min == 1) {
      min = 2;
    }

    int c, j, m, is_simple, k, z;
    for(int i = max; i >= min; i--) {

      c = i;
      for(j = 0; j < simples_n && c > 1; j++) {
        m = 0;
        while(c % simples[j] == 0) {
          c /= simples[j];
          m++;
        }

        if (m > counts[j]) {
          counts[j] = m;
        }
      }

      for(j = simples_n; c > 1; j++) {
        for(k = simples[j-1] + 1;;k++) {
          is_simple = 1;
          for(z = 0; z < j && is_simple; z++) {
            is_simple = k % simples[z];
          }
          if (is_simple) {
            simples[j] = k;
            break;
          }
        }

        m = 0;
        while(c % simples[j] == 0) {
          c /= simples[j];
          m++;
        }

        if (m > counts[j]) {
          counts[j] = m;
        }
      }

      simples_n = j;
    }

    int x, p, r = 1;

    for(int i = 0; i < simples_n; i++)
    {
        x = simples[i];
        p = counts[i];
        for(; p > 0; p >>= 1) {
          if (p & 1) {
            r *= x;
          }
          x *= x;
        }
    }
    return r;
}
