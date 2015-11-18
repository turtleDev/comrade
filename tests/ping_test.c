#include "../src/ping.h"
#include <stdio.h>

int main() {
    
    printf("---------------------------------------\n");
    printf("test 1: trying to ping 127.0.0.1\n");
    printf("ping returned: %d\n", ping("127.0.0.1", 4));

    printf("---------------------------------------\n");
    printf("test 2: trying to ping 255.255.255.255\n");
    printf("ping returned: %d\n", ping("255.255.255.255", 4));

    printf("---------------------------------------\n");
    printf("test 3: trying to ping www.google.com\n");
    printf("ping returned: %d\n", ping("www.google.com", 4));

    return 0;
}
