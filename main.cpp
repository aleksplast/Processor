#include <stdio.h>

#include "processor.h"

int main()
{
    FILE* fp = fopen("logs.txt", "w");
    fclose(fp);
    const char* input = "out.txt";

    Processor(input);

    return 0;
}
