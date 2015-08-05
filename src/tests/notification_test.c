#include "../notification.h"
#include <stdio.h>
#include "../lock.c"
#include <stdlib.h>
int main(int argc, char *argv[]) {

    struct Config cfg = {"Hello", "world", "test.wav","icon.png"};

    display_notification(&cfg);
    return 0;
}
