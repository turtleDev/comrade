#include <stdio.h>
#include <stdlib.h>

#include "../src/notification.h"
#include "../src/lock.c"

int main(int argc, char *argv[]) {

    struct Config cfg = {"Hello", "world", "test.wav","icon.png"};

    display_notification(&cfg);
    return 0;
}
