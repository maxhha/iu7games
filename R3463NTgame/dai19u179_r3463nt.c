#define LOG_LEVEL NO_LOG
#include "macrologger.h"

static char prev = 'B';

int reagent_game(char **bf, const int size)
{
    char target = prev;

    prev = prev == 'A' ? 'B' : 'A';

    {
        int side = -1;
        int len = size - 1;
        int i = 0;
        int x = 0, y = 0, dx = 1, dy = 0;
        int result = -1;

        while (1)
        {
            // LOG_DEBUG("side = %d, len = %d", side, len);
            LOG_DEBUG("x = %d, y = %d, i = %d", x, y, i);
            // LOG_DEBUG("dx = %d, dy = %d", dx, dy);
            LOG_DEBUG("------%s", "");

            if (bf[y][x] == target)
            {
                result = x + y * size;
            }

            x += dx;
            y += dy;
            i++;
            if (i >= len)
            {
                i = 0;
                side += 1;
                LOG_DEBUG("side = %d", side);
                int ndx = -dy;
                int ndy = dx;

                dx = ndx;
                dy = ndy;

                if (len == 0)
                    break;

                if (side == 2)
                {
                    len -= 1;
                    LOG_DEBUG("decreace len = %d", len);
                }

                if (side == 2)
                    side = 0;
            }
        }

        if (result != -1)
            return result;
    }

    target = target == 'A' ? 'B' : 'A';

    {
        int side = -1;
        int len = size - 1;
        int i = 0;
        int x = 0, y = 0, dx = 1, dy = 0;
        int result = -1;



        while (1)
        {
            // LOG_DEBUG("side = %d, len = %d", side, len);
            LOG_DEBUG("x = %d, y = %d, i = %d", x, y, i);
            // LOG_DEBUG("dx = %d, dy = %d", dx, dy);
            LOG_DEBUG("------%s", "");

            if (bf[y][x] == target)
            {
                result = x + y * size;

            }

            x += dx;
            y += dy;
            i++;
            if (i >= len)
            {
                i = 0;
                side += 1;
                LOG_DEBUG("side = %d", side);
                int ndx = -dy;
                int ndy = dx;

                dx = ndx;
                dy = ndy;

                if (len == 0)
                break;

                if (side == 2)
                {
                    len -= 1;
                    LOG_DEBUG("decreace len = %d", len);
                }

                if (side == 2)
                side = 0;
            }
        }
        if (result != -1)
            return result;
    }

    return 0;
}
