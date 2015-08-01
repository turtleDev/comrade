#include "../notification_sound.h"
#include <stdio.h>

int main() {
    char *f = "test.wav";
    
    printf("Playing sound\n");
    play_notification_sound(f);

    return 0;
}
