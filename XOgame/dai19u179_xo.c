#include <stdio.h>
#include <math.h>

int is_win(char **bf, const int field_size, int pos) {
    int x = pos % field_size;
    int y = pos / field_size;
    char M = *((char *) bf + pos);

    for(int j = 0; j < 4; j++) {
        int count = 1;
        int dx =
            j == 0 ? -1 :
            j == 1 ? 0 : 1;
        int dy =
            j == 0 ? -1 :
            j == 1 ? 1 :
            j == 2 ? 1 : 0;

        for(int i = 1; x + i * dx < field_size && y + i * dy < field_size && x + i * dx >= 0 && y + i * dy >= 0; i++) {
            if (*((char *) bf + x + dx*i + (y + dy*i) * field_size) == M) {
                count += 1;
            } else {
                break;
            }
        }
        dx = -dx;
        dy = -dy;
        for(int i = 1; x + i * dx < field_size && y + i * dy < field_size && x + i * dx >= 0 && y + i * dy >= 0; i++) {
            if (*((char *) bf + x + dx*i + (y + dy*i) * field_size) == M) {
                count += 1;
            } else {
                break;
            }
        }
        if (count == field_size) {
            return 1;
        }
    }
    return 0;
}

#define PLAYOUTS_NUMBER 200
#define CACHE_SIZE 200
#define EMPTY 0
#define MCTS_COEF 1.4
long long int cache_table[CACHE_SIZE];
int cache_table_visits[CACHE_SIZE];
int cache_table_wins[CACHE_SIZE];
int cache_table_child_visits[CACHE_SIZE];
int cache_table_child_wins[CACHE_SIZE];
int cache_table_update_code[CACHE_SIZE];
int cache_update_code = 0;
int cache_filled_size;

int char2int(char c){
    if (c == ' ') {
        return 0;
    }
    if (c == 'X') {
        return 2;
    }
    return 1;
}

typedef long long int hash;

hash field_hash(char *m, int field_size)
{
    hash r = 0;

    for(int i = 0; i < field_size * field_size; i++){
        r = r * 3 + char2int(*(m + i));
    }
    return r;
}

int is_made_from(hash src, hash dest)
{
    while(src > 0) {
        if (src % 3 > dest % 3) {
            return 0;
        }
        src /= 3;
        dest /= 3;
    }
    return 1;
}

void remove_cache_table(int i)
{
    cache_table[i] = EMPTY;
    cache_table_visits[i] = 0;
    cache_table_wins[i] = 0;
    cache_table_child_visits[i] = 0;
    cache_table_child_wins[i] = 0;
    cache_filled_size -= 1;
}

int find_cache_index(hash h) {
    for(int i = 0; i < CACHE_SIZE; i++) {
        if (cache_table[i] == h) {
            return i;
        }
    }
    return -1;
}

int add_hash_to_cache(hash h) {
    for(int i = 0; i < CACHE_SIZE; i++) {
        if (cache_table[i] == EMPTY) {
            cache_filled_size += 1;
            cache_table[i] = h;
            cache_table_update_code[i] = cache_update_code;
            return i;
        }
    }
    return -1;
}

int rand_numbers[25] = {
    20, 18, 15, 3, 9,
    24, 13, 5, 6, 17,
    16, 7, 1, 2, 19,
    21, 0, 23, 11, 4,
    10, 12, 8, 22, 14
};
int rand_numbers_i = 0;

int get_rand(void) {
    rand_numbers_i = (rand_numbers_i + 1) % 25;
    return rand_numbers[rand_numbers_i];
}

int get_random_finish_state(char *m, int field_size, char symb) {
    int step;
    int field_sqr = field_size*field_size;
    int has_space = 0;

    for(int i = 0; !has_space && i < field_sqr; i++) {
        has_space = *(m + i) == ' ';
    }

    if (!has_space) {
        return 0;
    }

    while(1) {
        step = get_rand() % (field_size * field_size);
        if (*(m + step) == ' ') {
            break;
        }
    }
    *(m + step) = symb;

    if (is_win((char**) m, field_size, step)) {
        *(m + step) = ' ';
        return 1;
    }

    int opponent = get_random_finish_state(m, field_size, symb == 'X' ? 'O' : 'X');
    *(m + step) = ' ';
    return -opponent;
}

void update_cache(char *m, int field_size, char symb)
{
    hash m_h = field_hash(m, field_size);
    int m_h_i = find_cache_index(m_h);

    if (cache_table_update_code[m_h_i] == cache_update_code) {
        return;
    }
    cache_table_update_code[m_h_i] = cache_update_code;
    cache_table_child_visits[m_h_i] = 0;
    cache_table_child_wins[m_h_i] = 0;

    int field_sqr = field_size*field_size;
    int count_empty = 0;
    for(int i = 0; i < field_sqr; i++) {
        count_empty += *(m + i) == ' ';
    }

    if (count_empty == 1) {
        int step = -1;
        for(int i = 0; i < field_sqr; i++) {
            if (*(m + i) == ' ') {
                step = i;
                break;
            }
        }
        *(m + step) = symb;
        hash h = field_hash(m, field_size);
        int h_i = find_cache_index(h);
        *(m + step) = ' ';

        if (h_i == -1) {
            return;
        }
        cache_table_child_visits[m_h_i] = cache_table_visits[h_i];
        cache_table_child_wins[m_h_i] = cache_table_wins[h_i];
        return;
    }

    for(int step1 = 0; step1 < field_sqr; step1++) {
        if (*(m + step1) != ' ') continue;
        for(int step2 = 0; step2 < field_sqr; step2++) {
            if (*(m + step2) != ' ') continue;
            *(m + step1) = symb;
            *(m + step2) = symb == 'X' ? 'O' : 'X';
            hash h = field_hash(m, field_size);
            int h_i = find_cache_index(h);

            if (h_i >= 0) {
                update_cache(m, field_size, symb);
                cache_table_child_visits[m_h_i] += cache_table_visits[h_i] + cache_table_child_visits[h_i];
                cache_table_child_wins[m_h_i] += cache_table_wins[h_i] + cache_table_child_wins[h_i];
            }

            *(m + step1) = ' ';
            *(m + step2) = ' ';
        }
    }
}

void mcts_playout(char *m, int field_size, char symb, int level) {
    int field_sqr = field_size*field_size;
    int count_empty = 0;
    for(int i = 0; i < field_sqr; i++) {
        count_empty += *(m + i) == ' ';
    }

    if (count_empty == 0) {
        return;
    }

    hash m_h = field_hash(m, field_size);
    int m_h_i = find_cache_index(m_h);

    if (count_empty == 1) {
        int step = -1;
        for(int i = 0; i < field_sqr; i++) {
            if (*(m + i) == ' ') {
                step = i;
                break;
            }
        }
        *(m + step) = symb;
        hash h = field_hash(m, field_size);
        int h_i = find_cache_index(h);
        if (h_i == -1) {
            h_i = add_hash_to_cache(h);
        }
        cache_table_visits[h_i] += 1;

        if (is_win((char **)m, field_size, step)) {
            cache_table_wins[h_i] += 2;
        } else {
            cache_table_wins[h_i] += 1;
        }
        *(m + step) = ' ';
        return;
    }

    double max_value = -1;
    int max_value_cache_i = -1;
    int max_value_step1 = -1;
    int max_value_step2 = -1;

    for(int i = 0; i < field_sqr; i++) {
        int step1 = -1;
        while(1) {
            step1 = get_rand() % field_sqr;
            if (*(m + step1) == ' ') {
                break;
            }
        }
        *(m + step1) = symb;

        int step2;
        while(1) {
            step2 = get_rand() % field_sqr;
            if (*(m + step2) == ' ') {
                break;
            }
        }
        *(m + step2) = symb == 'X' ? 'O' : 'X';

        hash h = field_hash(m, field_size);
        int cache_i = find_cache_index(h);

        *(m + step1) = ' ';
        *(m + step2) = ' ';

        if (cache_i == -1) {
            max_value_cache_i = add_hash_to_cache(h);
            max_value_step1 = step1;
            max_value_step2 = step2;
            break;
        }

        int sum_wins = cache_table_wins[cache_i] + cache_table_child_wins[i];
        int visits = cache_table_visits[cache_i] + cache_table_child_visits[cache_i];
        int parent_visits = cache_table_visits[m_h_i] + cache_table_child_visits[m_h_i];

        double exploration = (double) parent_visits / visits;

        double v = (double) sum_wins / 2.0 / visits + MCTS_COEF*sqrt(log(exploration));
        if (v > max_value) {
            max_value = v;
            max_value_cache_i = cache_i;
            max_value_step1 = step1;
            max_value_step2 = step2;
        }
    }

    if (cache_table_visits[max_value_cache_i] == 0) {
        *(m + max_value_step1) = symb;
        *(m + max_value_step2) = symb == 'X' ? 'O' : 'X';



        int result;
        if (is_win((char **)m, field_size, max_value_step1)) {
            result = 2;
        }
        else if (is_win((char **)m, field_size, max_value_step2)) {
            result = 0;
        }
        else {
            result = 1 + get_random_finish_state(m, field_size, symb);
        }
        *(m + max_value_step1) = ' ';
        *(m + max_value_step2) = ' ';


        cache_table_visits[max_value_cache_i] = 1;
        cache_table_wins[max_value_cache_i] = result;
        return;
    }
    *(m + max_value_step1) = symb;
    *(m + max_value_step2) = symb == 'X' ? 'O' : 'X';

    if (is_win((char **)m, field_size, max_value_step1)) {
        cache_table_visits[max_value_cache_i] += 1;
        cache_table_wins[max_value_cache_i] += 2;
        *(m + max_value_step1) = ' ';
        *(m + max_value_step2) = ' ';

        return;
    }
    else if (is_win((char **)m, field_size, max_value_step2)) {
        cache_table_visits[max_value_cache_i] += 1;

        *(m + max_value_step1) = ' ';
        *(m + max_value_step2) = ' ';

        return;
    }
    else if (count_empty == 2) {
        cache_table_visits[max_value_cache_i] += 1;
        cache_table_wins[max_value_cache_i] += 1;

        *(m + max_value_step1) = ' ';
        *(m + max_value_step2) = ' ';

        return;
    }

    mcts_playout(m, field_size, symb, level + 1);

    *(m + max_value_step1) = ' ';
    *(m + max_value_step2) = ' ';
}

int xogame(char **bf, const int field_size, const char symb) {
    char field[5][5];
    char *m = (char *) field;

    for(int y = 0; y < field_size; y++) {
        for(int x = 0; x < field_size; x++) {
            *(m + y*field_size + x) = *((char *)bf + y*field_size + x);
        }
    }

    hash h = field_hash(m, field_size);
    if (h == 0) {
        for(int i = 0; i < CACHE_SIZE; i++) {
            remove_cache_table(i);
        }
        cache_filled_size = 0;
        return field_size / 2 + field_size / 2 * field_size;
    }

    int empty_step = -1;

    for(int i = 0; i < field_size*field_size; i++) {
        if (*(m + i) == ' ') {
            if (empty_step == -1) {
                empty_step = i;
            }
            else {
                empty_step = -1;
                break;
            }
        }
    }

    if (empty_step >= 0) {
        return empty_step;
    }

    for(int i = 0; i < CACHE_SIZE; i++) {
        if (cache_table[i] != EMPTY && !is_made_from(h, cache_table[i])) {
            remove_cache_table(i);
        }
    }

    int h_i = find_cache_index(h);
    if (h_i == -1) {
        h_i = add_hash_to_cache(h);
    }

    for(int i = 0; cache_filled_size < CACHE_SIZE && i < PLAYOUTS_NUMBER; i++) {
        mcts_playout(m, field_size, symb, 2);
        cache_update_code++;
        update_cache(m, field_size, symb);
    }

    double max_value = -1;
    int max_value_step = -1;

    for(int step1 = 0; step1 < field_size * field_size; step1++) {
        if (*(m + step1) != ' ') continue;
        int wins = 0;
        int visits = 0;

        for(int step2 = 0; step2 < field_size * field_size; step2++) {
            if (*(m + step2) != ' ') continue;
            *(m + step1) = symb;
            *(m + step2) = symb == 'X' ? 'O' : 'X';

            hash h2 = field_hash(m, field_size);
            int h2_i = find_cache_index(h2);
            *(m + step1) = ' ';
            *(m + step2) = ' ';

            if (h2_i == -1) continue;

            wins += cache_table_wins[h2_i] + cache_table_child_wins[h2_i];
            visits += cache_table_visits[h2_i] + cache_table_child_visits[h2_i];
        }

        double v = (double) wins / 2.0 / visits;
        if (v > max_value) {
            max_value = v;
            max_value_step = step1;
        }
    }

    return max_value_step;
}
