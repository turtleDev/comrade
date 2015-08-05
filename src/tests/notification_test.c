#include "../notification.h"
#include <stdio.h>
#include "../lock.c"
int main() {
    printf("%s\n", __FILE__);
    _lock_acquire_linux(__FILE__);
    struct Config cfg = {"Hello", "world", "test.wav", "/home/pitchblack/Documents/logo2.png"};

    display_notification(&cfg);
    _lock_release_linux();
    return 0;
}
