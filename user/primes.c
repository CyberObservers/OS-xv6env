#include "kernel/types.h"
#include "user/user.h"


#if 0
void process(int pd[]);

int main(int argc, char** argv)
{
    int pd[2];
    pipe(pd);

    int pid;
    if((pid=fork())==0){
        process(pd);
        exit(0);
    }
    else{
        int pass[34];
        for(int i=0;i<34;i++){
            pass[i]=i+2;
        }
        close(pd[0]);
        write(pd[1],pass,sizeof(int)*34);
        wait(0);
    }
    exit(0);
}

void process(int pd[])
{
    int pid;//fork
    int prime;//本进程的质数
    int next;//读取的数
    int pass[34];//向下传递的数组
    int ptr=0;//数组指针
    int len;//read函数返回值
    int this_pd[2];//管道
    pipe(this_pd);

    close(pd[1]);
    close(this_pd[0]);
    len=read(pd[0],&prime,sizeof(prime));
    printf("prime %d\n",prime);

    while(len!=0){
        len=read(pd[0],&next,sizeof(int));
        if(next%prime!=0){
            pass[ptr++]=next;
        }
    }
    close(pd[0]);

    if(ptr==0){
        exit(0);
    }

    int tmp;
    for(int i=0;i<ptr;i++){
        tmp=pass[ptr];
        write(this_pd[1],&tmp,sizeof(int));
    }

    if((pid=fork())==0){
        process(this_pd);
    }
    
    wait(0);
}
#else

void func(int *input, int num){
	if(num == 1){
		printf("prime %d\n", *input);
		return;
	}
	int p[2],i;
	int prime = *input;
	int temp;
	printf("prime %d\n", prime);
	pipe(p);

    if(fork() == 0){
        for(i = 0; i < num; i++){
            temp = *(input + i);
			write(p[1], (char *)(&temp), 4);
		}
        exit(0);
    }
	close(p[1]);

    if(fork() == 0){
		int counter = 0;
		char buffer[4];
		while(read(p[0], buffer, 4) != 0){
			temp = *((int *)buffer);
			if(temp % prime != 0){
				*input = temp;
				input += 1;
				counter++;
			}
		}
		func(input - counter, counter);
		exit(0);
    }
	
	wait(0);
	wait(0);
}

int main(){
    int input[34];
	int i = 0;
	for(; i < 34; i++){
		input[i] = i+2;
	}
	func(input, 34);
    exit(0);
}
#endif
