#include "../notification_sound.h"
#include <stdio.h>
#include "../debug.h"
int main() {
    char *f = "test.wav";
    
    printf("Playing sound\n");
    play_notification_sound(f);

    printf("Checking error checking:\n");
    printf("trying to pass wrong arg: test.wav2\n");

    char *f2 = "test.wav2";
    if(play_notification_sound(f2) == -1) {
        log_err("Error opening %s", f2);
    }


    return 0;
}
