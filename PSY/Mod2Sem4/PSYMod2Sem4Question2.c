# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# define DUREE_MAX 5
# define CHECK_T(status , msg)				 		\
	if (0 != ( status )){ 						\
		fprintf (stderr , " pthread erreur : %s\n", msg);	\
		exit ( EXIT_FAILURE );					\
	}

void * monThread (void * arg);
void bye(void);
int main(int argc , char *argv [])
{
	pthread_t tid1 , tid2;/* Identifiants de thread*/
	atexit (bye);
	printf ("Le processus %d va créer deux threads \n",
	getpid ());
	CHECK_T (pthread_create (& tid1 , NULL , monThread , NULL)," pthread_create (1)");
	CHECK_T (pthread_create (& tid2 , NULL , monThread , NULL)," pthread_create (2)");
	printf ("\tatttente de la terminaison des threads \n");
	CHECK_T (pthread_join (tid1 , NULL)," pthread_join (1)");
	CHECK_T (pthread_join (tid2 , NULL)," pthread_join (2)");
	return EXIT_SUCCESS ;
	}
void * monThread (void * arg){
	printf (" Thread fils : \tDébut de traitement \n");
	sleep (rand () % DUREE_MAX + 1);
	printf (" Thread fils : \tFin de traitement \n");
	return NULL;
}

void bye (void)
{
	printf ("Fin du processus %d\n", getpid ());
}
