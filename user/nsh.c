#define MAXARGV 10
#define MAXWORD 30
#define MAXIN 100

#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int getcmd(char *buf, int nbuf);
void setargs(char *buf, char* argv[], int* argc);
void execPipe(char* argv[], int argc);
void runcmd(char* argv[], int argc);

char SPACEWORDS[] = " \t\r\n\v";
char args[MAXARGV][MAXWORD];

int main() {
    char buf[MAXIN];
    while (getcmd(buf, sizeof(buf)) >= 0) {
        if (fork() == 0) {
            char* argv[MAXARGV];
            int argc = -1;
            setargs(buf, argv, &argc);
            runcmd(argv, argc);
        }
        wait(0);
    }
    exit(0);
}
//将当前命令存入缓冲字符串
int getcmd(char *buf, int nbuf)
{
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0)
        return -1;
    return 0;
}

void setargs(char *buf, char* argv[], int* argc) {
    int i, j;
    for (i = 0; i < MAXARGV; i++) argv[i] = &args[i][0];
    for (i = 0, j = 0; buf[j] != '\n' && buf[j] != '\0'; j++, i++) {
        while (strchr(SPACEWORDS, buf[j])) { j++; }
        argv[i] = buf + j;
        while (strchr(SPACEWORDS, buf[j]) == 0) { j++; }
        buf[j] = '\0';
    }
    argv[i] = 0;
    *argc = i;
}

void execPipe(char* argv[], int argc){
    int i = 0;
    int fd[2];
    for( i = 0; i < argc; i++) {
        if(strcmp(argv[i], "|") == 0) {
            argv[i] = 0;
            break;
        }
    }
    pipe(fd);
    if(fork() == 0) {
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv, i);
    }
    else {
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        runcmd(argv+i+1, argc-i-1);
    }
}

void runcmd(char* argv[], int argc) {
    int i;
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], "|") == 0) {
            execPipe(argv, argc);
        }
    }
    for(i = 1; i < argc; i++) {
        if(strcmp(argv[i], ">") == 0) {
            close(1);
            open(argv[i+1], O_CREATE|O_WRONLY);
            argv[i] = 0;
        }
        if(strcmp(argv[i], "<") == 0) {
            close(0);
            open(argv[i+1], O_RDONLY);
            argv[i] = 0;
        }
    }
    exec(argv[0], argv);
}
