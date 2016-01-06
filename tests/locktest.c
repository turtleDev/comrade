#include <stdio.h>

#include "../src/lock.h"

int main(int argc, char *argv[]) {
    int rc = lock_acquire(argv[0]);

    if( rc != 0 ) {
        printf("Another instance is running\n");
        return -1;
    }

    printf("Press <enter> to exit.\n");
    getchar();

    lock_release();
}
