#define LOG_LEVEL NO_LOG
#include "macrologger.h"

#define FIELD_YSIZE 20
#define FIELD_XSIZE 10

#define FIG_SIZE 4

typedef char *figure_t;

static figure_t figure_array_J[4] = {
    "XJXX"
    "XJXX"
    "JJXX"
    "XXXX",

    "JXXX"
    "JJJX"
    "XXXX"
    "XXXX",

    "JJXX"
    "JXXX"
    "JXXX"
    "XXXX",

    "JJJX"
    "XXJX"
    "XXXX"
    "XXXX",

};
static figure_t figure_array_I[4] = {
    "IXXX"
    "IXXX"
    "IXXX"
    "IXXX",

    "IIII"
    "XXXX"
    "XXXX"
    "XXXX",

    "IXXX"
    "IXXX"
    "IXXX"
    "IXXX",

    "IIII"
    "XXXX"
    "XXXX"
    "XXXX",

};
static figure_t figure_array_O[4] = {
    "OOXX"
    "OOXX"
    "XXXX"
    "XXXX",

    "OOXX"
    "OOXX"
    "XXXX"
    "XXXX",

    "OOXX"
    "OOXX"
    "XXXX"
    "XXXX",

    "OOXX"
    "OOXX"
    "XXXX"
    "XXXX",

};
static figure_t figure_array_L[4] = {
    "LXXX"
    "LXXX"
    "LLXX"
    "XXXX",

    "LLLX"
    "LXXX"
    "XXXX"
    "XXXX",

    "LLXX"
    "XLXX"
    "XLXX"
    "XXXX",

    "XXLX"
    "LLLX"
    "XXXX"
    "XXXX",

};
static figure_t figure_array_Z[4] = {
    "ZZXX"
    "XZZX"
    "XXXX"
    "XXXX",

    "XZXX"
    "ZZXX"
    "ZXXX"
    "XXXX",

    "ZZXX"
    "XZZX"
    "XXXX"
    "XXXX",

    "XZXX"
    "ZZXX"
    "ZXXX"
    "XXXX",

};
static figure_t figure_array_T[4] = {
    "TTTX"
    "XTXX"
    "XXXX"
    "XXXX",

    "XTXX"
    "TTXX"
    "XTXX"
    "XXXX",

    "XTXX"
    "TTTX"
    "XXXX"
    "XXXX",

    "TXXX"
    "TTXX"
    "TXXX"
    "XXXX",

};
static figure_t figure_array_S[4] = {
    "XSSX"
    "SSXX"
    "XXXX"
    "XXXX",

    "SXXX"
    "SSXX"
    "XSXX"
    "XXXX",

    "XSSX"
    "SSXX"
    "XXXX"
    "XXXX",

    "SXXX"
    "SSXX"
    "XSXX"
    "XXXX",

};


figure_t *get_figure_array(const char figure)
{
    switch (figure) {
        case 'J': return figure_array_J;
        case 'I': return figure_array_I;
        case 'O': return figure_array_O;
        case 'L': return figure_array_L;
        case 'Z': return figure_array_Z;
        case 'T': return figure_array_T;
        case 'S': return figure_array_S;
        default:
        LOG_ERROR("unknown figure '%c'(%d)", figure, figure);
        return NULL;
    }
}

int count_holes_in_field(char **bf)
{
    int sum = 0;

    for (int x = 0; x < FIELD_XSIZE; x++)
    {
        int y = 0;
        while (y < FIELD_YSIZE && bf[y][x] == 'X')
            y++;

        while (y < FIELD_YSIZE)
        {
            sum += bf[y][x] == 'X';
            y++;
        }
    }

    return sum;
}

int get_height_at_column_in_field(char **bf, int x)
{
    int y = 0;
    while (y < FIELD_YSIZE && bf[y][x] == 'X')
        y++;

    return FIELD_YSIZE - y;
}

int get_max_height_in_field(char **bf)
{
    int max_h = 0;

    for (int x = 0; x < FIELD_XSIZE; x++)
    {
        int h = get_height_at_column_in_field(bf, x);
        if (h > max_h)
            max_h = h;
    }

    return max_h;
}

int can_place_fig(char **bf, figure_t fig, int xx, int yy)
{
    for (int y = 0; y < FIG_SIZE; y++)
        for (int x = 0; x < FIG_SIZE; x++)
        {
            if (fig[x + y * FIG_SIZE] == 'X')
                continue;

            if (x + xx >= FIELD_XSIZE || y + yy >= FIELD_YSIZE)
                return 0;

            if (bf[y + yy][x + xx] != 'X')
                return 0;
        }

    return 1;
}

void place_fig(char **bf, figure_t fig, int xx, int yy)
{
    for (int y = 0; y < FIG_SIZE; y++)
        for (int x = 0; x < FIG_SIZE; x++)
            if (fig[x + y * FIG_SIZE] != 'X')
                bf[y + yy][x + xx] = fig[x + y * FIG_SIZE];
}

void remove_fig(char **bf, figure_t fig, int xx, int yy)
{
    for (int y = 0; y < FIG_SIZE; y++)
        for (int x = 0; x < FIG_SIZE; x++)
            if (fig[x + y * FIG_SIZE] != 'X')
                bf[y + yy][x + xx] = 'X';
}

int count_removing_lines(char **bf)
{
    int r = 0;

    for (int y = 0; y < FIELD_YSIZE; y++)
    {
        int is_full = 1;

        for (int x = 0; x < FIELD_XSIZE && is_full; x++)
        {
            is_full = bf[y][x] != 'X';
        }

        r += is_full;
    }

    return r;
}

int tetris_game(char **bf, const char figure, int *const angle)
{
    int max_x = 0;
    int max_angle = 0;
    double max_h = -1e6;

    figure_t *fig_arr = get_figure_array(figure);

    for (int x = 0; x < FIELD_XSIZE; x++)
        for (int angl = 0; angl < 4; angl++)
        {
            figure_t fig = fig_arr[angl];

            int y = 0;

            while (can_place_fig(bf, fig, x, y))
                y++;

            y--;

            if (y < 0)
                continue;

            LOG_DEBUG("place fig%c[%d] at %d, %d", figure, angl, x, y);
            place_fig(bf, fig, x, y);

            int holes_n = count_holes_in_field(bf);
            int lines_n = count_removing_lines(bf);
            int max_height = get_max_height_in_field(bf);
            int rcol_height = get_height_at_column_in_field(bf, FIELD_XSIZE - 1);

            LOG_DEBUG("holes_n = %d", holes_n);
            LOG_DEBUG("lines_n = %d", lines_n);
            LOG_DEBUG("max_height = %d", max_height);
            LOG_DEBUG("rcol_height = %d", rcol_height);

            #define HOLES_K -79
            #define LINES_K 75
            #define MAX_HEIGHT_K -90
            #define RCOL_HEIGHT_K 7

            double h = 0;
            h += holes_n * HOLES_K;
            h += lines_n * LINES_K;
            h += max_height * MAX_HEIGHT_K;
            h += rcol_height * RCOL_HEIGHT_K;

            remove_fig(bf, fig, x, y);

            LOG_DEBUG("heuristic %lf", h);

            if (h > max_h)
            {
                max_h = h;
                max_x = x;
                max_angle = angl;
            }
        }

    LOG_DEBUG("RESULT: h = %lf;", max_h);
    LOG_DEBUG("RESULT: x = %d; angle = %d", max_x, max_angle);

    *angle = max_angle * 3;

    return max_x;
}
