int split(const char *string, char **matrix, const char symbol)
{
    const char *c = string;
    char *word = matrix[0];
    int word_len = 0;
    int n = 0;

    while(1)
    {
        if (*c == '\0')
        {
            if (word_len == 0)
            {
                break;
            }
            n++;
            *word = '\0';
            break;
        }

        if (*c != symbol)
        {
            word_len++;
            *(word++) = *c;
        }
        else if (word_len > 0)
        {
            word_len = 0;
            *word = '\0';
            word = matrix[++n];
        }
        c++;
    }

    return n;
}
