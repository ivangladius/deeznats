#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define handle(str) \
do {perror(str);exit(EXIT_FAILURE);}while(0)

int main(int argc, char ** argv) {

    if (argc != 2)
        handle("not enough arguments!\n");
    printf("Hello, %s!\n", argv[1]);
    return 0;
}
