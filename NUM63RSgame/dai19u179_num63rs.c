#include <stdio.h>

int numbers_game(int min, int max)
{
    for(int i = max;;i++)
    {
        int passed = 1;
        for(int j = min; j <= max && passed; j++)
        {
            if (i % j != 0)
            {
               passed = 0;
            }
        }
        if (passed)
        {
            return i;
        }
    }
}
