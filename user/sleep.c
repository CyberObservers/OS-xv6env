#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char ** argv)
{
    if(argc==1){
        printf("Usage: sleep <sleep_time>\n");
        exit(-1);
    }
    else{
        int duration=atoi(argv[1])*10;
        sleep(duration);
        exit(0);
    }

    exit(0);
}