#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char* argv[], char *envp[]){
	int i;
	int somme = 0;

	printf("\nJe suis le processus ................ n %d\n", getpid());
	printf(  "Mon pere est le processus ........... n %d\n", getppid());

	for(i = 0;argv[i] != NULL; i++){
		somme += atoi(argv[i]);
	}
	return somme;
}
