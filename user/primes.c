#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void sieve(int read_fd)
{
    int pipe_fd[2];
    int pid;
    int prime;

    if(read(read_fd,&prime,sizeof(int))<=0)
    {
        close(read_fd);
        exit(0);
    }
    printf("prime %d\n",prime);
    if(pipe(pipe_fd)<0)
    {
        fprintf(2,"pipe execute failed");
        exit(1);
    }
    pid=fork();
    if(pid==0)
    {
        close(pipe_fd[1]);
        sieve(pipe_fd[0]);
    }
    else
    {
        close(pipe_fd[0]);
        int num;
        while(read(read_fd,&num,sizeof(int))>0)
        {
            if(num%prime!=0)
            {
                write(pipe_fd[1],&num,sizeof(int));
            }
        }
        close(pipe_fd[1]);
        close(read_fd);
        int state;
        wait(&state);
        exit(0);
    }
}

int main(int argc, char *argv())
{
    int state;
    int pipe_fd[2];
    int pid;
    if (pipe(pipe_fd) < 0)
    {
        fprintf(2, "pipe execute failed");
        exit(1);
    }
    pid = fork();
    if (pid < 0)
    {
        fprintf(2, "fork execute failed");
        exit(1);
    }
    else if (pid == 0)
    {
        close(pipe_fd[1]);
        sieve(pipe_fd[0]);
    }
    else
    {
        close(pipe_fd[0]);
        for(int i=2;i<=35;i++)
        {
            write(pipe_fd[1],&i,sizeof(int));
        }
        close(pipe_fd[1]);
        wait(&state);
        if(state==0)
        {
            exit(0);
        }
        else
        {
            fprintf(2,"child process failed");
            exit(0);
        }
    }
    return 0;
}