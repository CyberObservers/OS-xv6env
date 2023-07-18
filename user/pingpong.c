#include "kernel/types.h"
#include "user/user.h"
#include "kernel/stat.h"
#define CAPACITY 8
int turn;
int IsBusy[2]={0,0};

int main(int argc, char **argv)
{
    char buf[CAPACITY];
    int parent_fd[2], child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);
    int pid;

    if((pid=fork())==0){
        close(parent_fd[1]);//关闭parent的写端
        if(read(parent_fd[0],buf,CAPACITY)!=CAPACITY){
            printf("child read exception.\n");
            exit(1);
        }
        printf("%d: received %s\n", getpid(), buf);
        close(child_fd[0]);//关闭child的读端
        if(write(child_fd[1], "pong", CAPACITY)!=CAPACITY){
            printf("child write exception.\n");
            exit(2);
        }

        //IsBusy[0]=1;
        //turn=1;
        //while(IsBusy[1]==1 && turn==1);
        //printf("%d: received %s\n", getpid(), buf);
        //IsBusy[0]=0;

        exit(0);
    } else {
        close(parent_fd[0]);
        if(write(parent_fd[1],"ping",CAPACITY)!=CAPACITY){
            printf("father write exception.\n");
            exit(2);
        }
        close(child_fd[1]);
        if(read(child_fd[0],buf,CAPACITY)!=CAPACITY){
            printf("father read exception.\n");
            exit(1);
        }
        

        //IsBusy[1]=1;
        //turn=0;
        //while(IsBusy[0]==1 && turn==0);
        printf("%d: received %s\n", getpid(), buf);
        //IsBusy[1]=0;

        exit(0);
    }

    exit(0);
}