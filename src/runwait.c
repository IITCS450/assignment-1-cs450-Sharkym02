#include "common.h"
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void usage(const char *a){fprintf(stderr,"Usage: %s <cmd> [args]\n",a); exit(1);}

static double d(struct timespec a, struct timespec b){

 return (b.tv_sec-a.tv_sec)+(b.tv_nsec-a.tv_nsec)/1e9;}

int main(int c,char**v){

        if(c < 2)
                usage(v[0]);

        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);

        pid_t pid = fork();
        if(pid == 0){
                execvp(v[1], &v[1]);
                perror("fork error");
                exit(127);
        }

        int status;

        clock_gettime(CLOCK_MONOTONIC, &end);

        printf("Child PID: %d\n", pid);

        if(WIFEXITED(status)){
                printf("Exit code: %d\n", WEXITSTATUS(status));
        }
        else if(WIFSIGNALED(status)) {
                printf("Terminated by signal: %d\n", WTERMSIG(status));
        }

        printf("Elapsed time: %.3f seconds\n", d(start, end));

        return 0;
}