#include <stdio.h>
#include "../src/config.h"


void test_cfg(const char *string) {
    struct Config *cfg = config_load(string);
    
    printf("message: '%s'\ntitle: '%s'\nicon_file: '%s'\nsound_file: '%s'\n\
addr: '%s'\ncount: '%d'\n",
            cfg->message, cfg->title, cfg->icon_file, cfg->sound_file,
            cfg->address, cfg->ping_count);

    config_cleanup(cfg);

}

int main(void) {
    test_cfg("message=Hello, World!\n\
               title=morning\n\
               icon_file=data/icon.png\n\
               sound_file=data/sound.wav\n\
               address=127.0.0.1\n\
               ping_count=4");
    return 0;
}
