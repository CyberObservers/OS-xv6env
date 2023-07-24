#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char **argv)
{
    if(argc!=2){
        printf("usage: %s <sleep_time>\n", *argv);
        exit(1);
    }

    sleep(atoi(argv[1]));
    exit(0);
}