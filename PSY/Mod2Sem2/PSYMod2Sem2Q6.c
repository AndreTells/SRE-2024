# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <sys/types.h>
# include <errno.h>

void displayWhoami (void)
{
	printf ("Je suis le processus .............. n°%d\n", getpid ());
	printf ("Mon père est le processus ......... n°%d\n", getppid ());
}

int main(void)
{
	pid_t pidFils ;
	pidFils = fork ();
	/* Demande de création d'un processus */
	switch ( pidFils ) {
		case -1 :
			perror (" Échec de la création d'un processus fils");
			exit ( pidFils );
		case 0 :
			/* On met ici le code spécifique du processus fils */
			displayWhoami();
			exit (0); /* Fin normale du fils */
	
		default :
			displayWhoami();
			printf("Je suis le pere du processus ...... n %d\n", pidFils);
			sleep(10);
			/* On met ici la suite du code du processus père */
		break ;
	}
	return 0; /* Fin normale du processus père*/
}
