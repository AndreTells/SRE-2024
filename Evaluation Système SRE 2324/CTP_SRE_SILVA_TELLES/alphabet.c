# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <errno.h>
# include <unistd.h>
# include <sys/wait.h>
# include <semaphore.h>
# include <sys/fcntl.h>

#define CHECK(sts , msg) \
if (-1 == (sts)) { \
perror (msg); \
exit( EXIT_FAILURE ); \
}

#define SIZE_ALPHABET 26


void writeAlphabet(const char base, sem_t* blocker, sem_t* target);
sem_t* mon_semaphoreMin;
sem_t* mon_semaphoreMaj;

int main (int argc , char *argv [])
{

    pid_t pidFils;
    pidFils = fork();
    mon_semaphoreMin = sem_open( "alphabetSemMin_", O_CREAT|O_EXCL , 0660, 1);
    mon_semaphoreMaj = sem_open( "alphabetSemMaj_", O_CREAT|O_EXCL , 0660, 1);


    sem_init( mon_semaphoreMaj, 1,0);
    sem_init( mon_semaphoreMin, 1,1);
    switch ( pidFils ) {
        case -1 :
            perror ("Échec de la création d'un processus fils");
            exit ( pidFils );
        case 0 :
            /* On met ici le code spécifique du processus fils*/
	    // processus 1--> ecrit characters minuscules
	    writeAlphabet('a',mon_semaphoreMin, mon_semaphoreMaj);
            exit (0); /* Fin normale du fils */
        default :
	    // processus 2--> ecrit characetr majuscules
	    writeAlphabet('A',mon_semaphoreMaj, mon_semaphoreMin);
            break;
    }

    wait(NULL);
    sem_unlink("alphabetSemMin_"); 
    sem_unlink("alphabetSemMaj_"); 
    exit( EXIT_SUCCESS );
}


void writeAlphabet(const char base, sem_t* blocker, sem_t* target){
	for(int i=0; i< SIZE_ALPHABET; i++){
		sem_wait(blocker);
		printf("%c",base+i);	
		sem_post(target);
	}
}
