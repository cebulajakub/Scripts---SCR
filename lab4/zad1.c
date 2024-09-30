#include <stdio.h>
#include <stdlib.h>

int main() {
    char *username = getenv("LOGNAME");
        printf("Witaj %s!\n", username);
    return 0;
}

