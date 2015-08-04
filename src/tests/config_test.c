#include <stdio.h>
#include "../config.h"


void test_json(const char *string) {
    struct Config *cfg = config_load(string);
    
    printf("message: %s\ntitle: %s\nicon_file: %s\nsound_file: %s\n",
            cfg->message, cfg->title, cfg->icon_file, cfg->sound_file);

    config_cleanup(cfg);

}

int main(void) {
    test_json("{\"message\":\"Hello, World!\", \
                \"title\":\"morning\", \
                \"icon_file\":\"data/icon.png\", \
                \"sound_file\":\"data/sound.wav\"}");
    return 0;
}
