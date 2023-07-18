#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    int i, j = 0, ret, l, m = 0;
    char block[32], buffer[32];
    char *p = buffer;
    char *lineSplit[32];
    for(i = 1; i < argc; i++){
        lineSplit[j++] = argv[i];
    }
    while( (ret = read(0, block, sizeof(block))) > 0){
        for(l = 0; l < ret; l++){
            if(block[l] == '\n'){
                buffer[m] = 0;
                m = 0;
                lineSplit[j++] = p;
                p = buffer;
                lineSplit[j] = 0;
                j = argc - 1;
                if(fork() == 0){
                    exec(argv[1], lineSplit);
                }                
                wait(0);
            }
            else if(block[l] == ' ') {
                buffer[m++] = 0;
                lineSplit[j++] = p;
                p = &buffer[m];
            }
            else {
                buffer[m++] = block[l];
            }
        }
    }
    exit(0);
}
