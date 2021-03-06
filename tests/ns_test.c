#include <stdio.h>

#include "../src/notification_sound.h"
#include "../src/debug.h"

int main() {
    char *f = "test.wav";
    
    printf("Playing sound\n");
    notification_sound_play(f);

    printf("Checking error checking:\n");
    printf("trying to pass wrong arg: test.wav2\n");

    char *f3 = "test.wav2";
    if(notification_sound_play(f3) == -1) {
        log_err("Error opening %s", f3);
    }


    return 0;
}
