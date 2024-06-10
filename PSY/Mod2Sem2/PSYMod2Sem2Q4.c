#include <unistd.h>
#include <stdio.h>

int main(){
	printf("Je suis le processus .............n %d\n", getpid());
	printf("Mon pere est le processus ........n %d\n", getppid());
	return 0;
}
