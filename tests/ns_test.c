#include <stdio.h>

#include "../src/notification_sound.h"
#include "../src/debug.h"

int main() {
    char *f = "test.wav";
    
    printf("Playing sound\n");
    play_notification_sound(f);

    printf("Playing Music\n");
    char *f2 = "/home/pitchblack/Programming/Projects/Comrade/tests/song.mp3";
    play_notification_sound(f2);

    printf("Checking error checking:\n");
    printf("trying to pass wrong arg: test.wav2\n");

    char *f3 = "test.wav2";
    if(play_notification_sound(f3) == -1) {
        log_err("Error opening %s", f3);
    }


    return 0;
}
