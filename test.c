#include "test.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>


#if defined(_MSC_VER)

#define BIG_CONSTANT(x) (x)

#else

#define BIG_CONSTANT(x) (x##LLU)

#endif

// uint32_t seed = #

// int len = #


int main(int argc, char* argv[]){
    int carrot = atoi(argv[1]) ^ atoi(argv[2]);
    printf("Result: %d\n", carrot);
    return 0;
}
