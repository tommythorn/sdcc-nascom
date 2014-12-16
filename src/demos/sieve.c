#include <stdio.h>

#define N 6000

static char cand[N];

int main()
{
    int i;
    char *j;

    for (i = 2; i < N; ++i) {
        if (!cand[i]) {
            printf(" %d", i);
            j = cand + i;
            if (j < cand + N)
                do {
                    *j = 1;
                    j += i;
                } while (j < cand + N);
        }
    }

    return 0;
}
