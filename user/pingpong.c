#include <kernel/types.h>
#include <user/user.h>

int main(){

    int parent[2],child[2];
    char buf[64];

    pipe(parent);
    pipe(child);

    if (fork() == 0) {
        read(parent[0],buf,4);
        printf("%d: received %s\n", getpid(), buf);
        write(child[1], "pong", 4);
    }
    else {
        write(parent[1], "ping", 4);
        read(child[0],buf,4);
        printf("%d: received %s\n", getpid(), buf);
    }
    
	exit();
}