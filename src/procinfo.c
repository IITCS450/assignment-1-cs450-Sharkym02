#include "common.h"
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>

static void usage(const char *a){fprintf(stderr,"Usage: %s <pid>\n",a); exit(1);}

static int isnum(const char*s){for(;*s;s++) if(!isdigit(*s)) return 0; return 1;}

int main(int c,char**v){
        char path[1024];
        char buf[1024];
        FILE* fp;
        char state;
        int ppid;
        long utime, stime;
        char line[1024];
        char vmrss[1024] = "N/A";
        char cmd[1024];

        if(c!=2||!isnum(v[1])) usage(v[0]);

        snprintf(path , sizeof(path), "/proc/%s/stat", v[1]);
        fp = fopen(path, "r");
        if(!fp){
                if(errno == ENOENT) {
                        fprintf(stderr, "PID not found\n");
                }
                else if(errno == EACCES) {
                        fprintf(stderr, "Permission denied\n");
                }
                exit(1);
        }

        fgets(buf, sizeof(buf), fp);
        fclose(fp);

        char* rp = strrchr(buf, ')');

        sscanf(rp + 2, "%c %d %*s %*s %*s %*s %*s %*s %*s %*s %*s %ld %ld", &state, &ppid, &utime, &stime);

        long clk = sysconf(_SC_CLK_TCK);
        double cpu = (double)(utime + stime) / clk;

        snprintf(path, sizeof(path), "/proc/%s/status", v[1]);
        fp = fopen(path, "r");

        while(fgets(line, sizeof(line), fp)){
                if(strncmp(line, "VmRSS:", 6) == 0){
                        sscanf(line + 6, "%63[^\n]", vmrss);
                        break;
                }
        }
        fclose(fp);

        snprintf(path, sizeof(path), "/proc/%s/cmdline", v[1]);
        fp = fopen(path, "r");
        size_t n = fread(cmd, 1, sizeof(cmd) - 1, fp);
        fclose(fp);

        if(n == 0){
                strcpy(cmd, "[kernel thread]");
        }
        else{
                for(size_t i = 0; i < n - 1; i++){
                        if(cmd[i] == '\0'){
                                cmd[i] = ' ';
                        }
                }
                cmd[n] = '\0';
        }

        printf("Process state : %c\n", state);
        printf("Parent PID : : %d\n", ppid);
        printf("Command line : %s\n", cmd);
        printf("CPU time : %.2f seconds\n", cpu);
        printf("Resident memory usage : %s\n", vmrss);

        return 0;
}