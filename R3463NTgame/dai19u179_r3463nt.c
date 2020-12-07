#include <stdlib.h>
#define LOG_LEVEL INFO_LEVEL
#include "macrologger.h"

int splash_bomb(int move, char **field, const int field_size)
{
    int count_explosions = 0;
    int row = move / field_size;
    int column = move % field_size;

    char reagent = field[row][column];
    char *replacement_string = field[row];

    if (reagent == 'A')
    {
        replacement_string[column] = 'B';
    }
    else if (reagent == 'B')
    {
        replacement_string[column] = 'O';
        count_explosions++;

        if (column - 1 >= 0)
            count_explosions += splash_bomb(move - 1, field, field_size);

        if (column + 1 < field_size)
            count_explosions += splash_bomb(move + 1, field, field_size);

        if (row - 1 >= 0)
            count_explosions += splash_bomb(move - field_size, field, field_size);

        if (row + 1  < field_size)
            count_explosions += splash_bomb(move + field_size, field, field_size);
    }

    return count_explosions;
}

char **copy_bf(char **bf, const int size)
{
    char **rows = malloc(sizeof(char *) * size);

    if (!rows)
        LOG_ERROR("fail to malloc%s", "");

    for (int i = 0; i < size; i++)
    {
        rows[i] = malloc(size * sizeof(char));
        if (!rows[i])
            LOG_ERROR("fail to malloc%s", "");
        memcpy(rows[i], bf[i], size * sizeof(char));
    }
    return rows;
}

void free_bf(char **bf, const int size)
{
    for (int i = 0; i < size; i++)
        free(bf[i]);

    free(bf);
}

int is_empty_bf(char **bf, int size)
{
    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
            if (bf[y][x] != 'O')
                return 0;
    return 1;
}

int get_group(int *map, const int size, int pos)
{
    while (map[pos] != pos)
        pos = map[pos];

    return pos;
}

int *calc_groups_map(char **bf, const int size)
{
    int *m = malloc(size * size * sizeof(int));

    if (!m)
        LOG_ERROR("fail to malloc%s", "");

    for (int i = 0; i < size * size; i++)
        m[i] = i;

    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
        {
            int p = x + y * size;

            if (bf[y][x] != 'B')
                continue;

            int left = x > 0 && bf[y][x - 1] == 'B';
            int top = y > 0 && bf[y - 1][x] == 'B';

            if (top && left)
            {
                int left_g = get_group(m, size, p - 1);
                int top_g = get_group(m, size, p - size);

                m[p] = m[top_g] = left_g;
            }
            else if (top)
            {
                m[p] = get_group(m, size, p - size);
            }
            else if (left)
            {
                m[p] = get_group(m, size, p - 1);
            }
        }

    for (int i = 0; i < size * size; i++)
        m[i] = get_group(m, size, i);

    return m;
}

typedef struct { int move; int score; } result_t;

result_t get_outstanding_move(char **bf, const int size, int strategic_level, int prev_move)
{
    result_t result = {
        .move = -1,
        .score = 0,
    };

    if (strategic_level < 1 || is_empty_bf(bf, size))
        return result;

    result.score = -5000;

    LOG_DEBUG("create groups_map%s", "");
    int *groups_map = calc_groups_map(bf, size);
    if (!groups_map)
        LOG_ERROR("fail to malloc%s", "");

    LOG_DEBUG("create visited_groups%s", "");
    char *visited_groups = malloc(size * size * sizeof(char));
    if (!visited_groups)
        LOG_ERROR("fail to malloc%s", "");

    for (int i = 0; i < size * size; i++)
        visited_groups[i] = 0;

    for (int y = 0; y < size; y++)
        for (int x = 0; x < size; x++)
        {
            LOG_DEBUG("%2d    %2d    %2d", x, y, strategic_level);

            LOG_DEBUG("    check O%s", "");

            if (bf[y][x] == 'O')
                continue;

            int move = x + y * size;

            LOG_DEBUG("    check A%s", "");

            if (bf[y][x] == 'A' && move < prev_move)
                continue;

            LOG_DEBUG("    check group%s", "");


            if (visited_groups[groups_map[move]])
                continue;

            visited_groups[groups_map[move]] = 1;

            LOG_DEBUG("    create copy%s", "");

            char **nbf = copy_bf(bf, size);

            LOG_DEBUG("    splash_bomb%s", "");

            int score = splash_bomb(move, nbf, size) - 1;

            LOG_DEBUG("    simulate%s", "");
            score += get_outstanding_move(nbf, size, strategic_level - 1, move).score;
            LOG_DEBUG("    finish simulation%s", "");

            free_bf(nbf, size);

            if (score > result.score)
            {
                result.score = score;
                result.move = move;
            }
        }

    LOG_DEBUG("free visited_groups%s", "");
    free(visited_groups);
    LOG_DEBUG("free groups_map%s", "");
    free(groups_map);

    // LOG_DEBUG("score = %d", result.score);
    return result;
}

int reagent_game(char **bf, const int size)
{
    LOG_DEBUG(" x     y     l%s", "");

    result_t result = get_outstanding_move(bf, size, 5, -1);

    LOG_INFO("score = %d", result.score);

    return result.move;
}

// int main(void)
// {
//     int size = 0;
//
//     if (scanf("%d", &size) != 1 || size < 1)
//         return 1;
//
//     char **rows = malloc(sizeof(char *) * size);
//
//     if (!rows)
//         LOG_ERROR("fail to malloc%s", "");
//
//     for (int i = 0; i < size; i++)
//     {
//         rows[i] = malloc(size * sizeof(char));
//         if (!rows[i])
//             LOG_ERROR("fail to malloc%s", "");
//
//         for (int j = 0; j < size; j++)
//         {
//             int c = getchar();
//             while (c > 1 && !(c == 'O' || c == 'B' || c == 'A' || c == 'q'))
//                 c = getchar();
//
//             if (c == 'q')
//                 exit(1);
//
//             rows[i][j] = c;
//         }
//     }
//
//     reagent_game(rows, size);
//
//     free_bf(rows, size);
//
//     return 0;
// }
