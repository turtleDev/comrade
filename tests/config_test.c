#include <stdio.h>
#include "../src/config.h"


void test_json(const char *string) {
    struct Config *cfg = config_load(string);
    
    printf("message: %s\ntitle: %s\nicon_file: %s\nsound_file: %s\n\
addr: %s\ncount: %d\n",
            cfg->message, cfg->title, cfg->icon_file, cfg->sound_file,
            cfg->address, cfg->ping_count);

    config_cleanup(cfg);

}

int main(void) {
    test_json("{\"message\":\"Hello, World!\", \
                \"title\":\"morning\", \
                \"icon_file\":\"data/icon.png\", \
                \"sound_file\":\"data/sound.wav\", \
                \"address\": 127.0.0.1, \
                \"ping_count\":4 }");
    return 0;
}
