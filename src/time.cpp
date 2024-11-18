
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main (int, const char* argv[])
    {
    int time = clock();

    system (argv[1]);

    printf ("\n" "Time = %lg s\n", (double) (clock() - time) / CLK_TCK);
    }


