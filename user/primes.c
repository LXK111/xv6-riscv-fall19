#include "kernel/types.h"
#include "user/user.h"

void fun_primes(int *input, int num);

int main(){
    int input[34];
	int i = 0;
	for( i = 0; i < 34; i++) {
		input[i] = i+2;
	}

	fun_primes(input, 34);

    exit(0);
}

void fun_primes(int *input, int num) {

	if(num == 1) {
		printf("prime %d\n", *input);
		return;
	}

    int p[2],i;
	int prime = *input;
	int temp;
	
	printf("prime %d\n", prime);
	pipe(p);

    if(fork() == 0) {
        for(i = 0; i < num; i++) {
            temp = *(input + i);
			write(p[1], (char *)(&temp), 4);
		}
        exit(0);
    }

	close(p[1]);

	if(fork() == 0) {
		int count = 0;
		char buffer[4];
		while(read(p[0], buffer, 4) != 0) {
			temp = *((int *)buffer);
			if(temp % prime != 0) {
				*input = temp;
				input += 1;
				count++;
			}
		}
		fun_primes(input - count, count);
		exit(0);
    }

	wait(0);
	wait(0);
}
