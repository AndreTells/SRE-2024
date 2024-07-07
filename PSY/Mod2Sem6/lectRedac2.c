/* ------------------------------------------------------------------------ */
/*                   E N T Ê T E S    S T A N D A R D S                     */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <sched.h>

/* ------------------------------------------------------------------------ */
/*              C O N S T A N T E S     S Y M B O L I Q U E S               */
/* ------------------------------------------------------------------------ */
#define NBMAX_BUFFER	3
#define MAX_VALUE	    10
#define OVER            (-1)

#define TEMPO_1ms       1000000L
#define MAXLEN          255

#define NBLECTEURS 3

/* ------------------------------------------------------------------------ */
/*              D É F I N I T I O N S   D E   T Y P E S                     */
/* ------------------------------------------------------------------------ */
typedef void * (*pf_t) (void *);

typedef struct {              /* buffer circulaire d'entiers                */
    int buffer[NBMAX_BUFFER]; /* la zone de stockage                        */
    int nReads[NBMAX_BUFFER]; /* nombre de fois qu'un position du buffer a  */ 
			      /*  ete lu				    */
    sem_t semAcessReads[NBMAX_BUFFER]; /* mutex pour acceder le buffer      */
    				       /* nReads                            */
    sem_t semBufferPlein[NBLECTEURS];
    sem_t semBufferVide;
    /*  ............................ à compléter  ......................... */
} circBuffer_t;

/* ------------------------------------------------------------------------ */
/*                      M A C R O - F O N C T I O N S                       */
/* ------------------------------------------------------------------------ */
#define CHECK_T(status, msg)                                                 \
  if (0 != (status))   {                                                     \
    fprintf(stderr, "pthread erreur : %s\n", msg);                           \
    exit (EXIT_FAILURE);                                                     \
  }

#define CHECK(status, msg)                                                   \
    if (-1 == (status)) {                                                    \
        perror(msg);                                                         \
        exit(EXIT_FAILURE);                                                  \
    }

#define SYNCHRONOUS_CALL(sem, call) 					     \
	CHECK(sem_wait(&sem),"wait for a SYNCHRONOUS_CALL");		     \
	call; 								     \
	CHECK(sem_post(&sem), "post after a SYNCHRONOUS_CALL")
/* ------------------------------------------------------------------------ */
/*                 V A R I A B L E S    G L O B A L E S                     */
/* ------------------------------------------------------------------------ */
circBuffer_t buf;

sem_t semPrintf;

int numReads=0;
/* ------------------------------------------------------------------------ */
/*            P R O T O T Y P E S    D E    F O N C T I O N S               */
/* ------------------------------------------------------------------------ */
void   redacteur(long noR);
void   lecteur(long noL);
void   lecteurManager();
void   init(circBuffer_t * b);
void   tempo(int minDelai, int maxDelai);
void   putLine (const char *s);

/* ------------------------------------------------------------------------ */
int main(int argc, char * argv[])
{
#if !defined(NOREADER)
    pthread_t tidLecteur[NBLECTEURS];
#endif

#if !defined(NOWRITER)
    pthread_t tidRedact;
#endif

    printf("Lancement du programme d'id   = 0x%08lx\n",
           (long)pthread_self());

    srand(time(NULL));
    init(&buf);
    CHECK(sem_init(&semPrintf,0,1), "sem_init sur semPrintf");

#if !defined(NOREADER)
    for(int i=0;i<NBLECTEURS;i++) CHECK_T(pthread_create(&(tidLecteur[i]), NULL, (pf_t)lecteur,(void*)((long) i)),"pthread_create(lecteur)");
#endif

#if !defined(NOWRITER)
    CHECK_T(pthread_create(&tidRedact, NULL, (pf_t)redacteur, 0),
            "pthread_create(redacteur)");
#endif

#if !defined(NOREADER)
    for(int i=0;i<NBLECTEURS;i++) CHECK_T(pthread_join(tidLecteur[i], NULL), "pthread_join(lecteur)");
#endif

#if !defined(NOWRITER)
    CHECK_T(pthread_join(tidRedact, NULL), "pthread_join(redacteur)");
#endif

    CHECK(sem_destroy(&(buf.semBufferVide)),"sem_destroy() sur semBufferVide");
    CHECK(sem_destroy(&semPrintf), "sem_destroy() sur semPrintf");
    for(int i=0;i<NBLECTEURS; i++) CHECK(sem_destroy(&(buf.semBufferPlein[i])), "sem_destroy() sur semBufferPlein");
    return 0;
}

/* ------------------------------------------------------------------------ */
void  redacteur(long no)
{
    int i = 0, n = 0;
    char line[MAXLEN + 1];
	
    SYNCHRONOUS_CALL(
	semPrintf,
    	printf("Lancement du rédacteur %ld d'id = 0x%08lx\n",no, (long)pthread_self());
    	fflush(stdout)
    );

    while  ( n != OVER ) {
	CHECK(sem_wait(&(buf.semBufferVide)), "sem_wait() sur semBufferVide");
	//writes in buffer
        if (n == MAX_VALUE)
            buf.buffer[i] = n = OVER;
        else
            buf.buffer[i] = n++;
	// resets position's read count
	buf.nReads[i] = 0;

	//prints to terminal teh position has been written in
        sprintf(line,"Le rédacteur n°%ld écrit %d dans le buffer %d\n",
                no, buf.buffer[i], i);
        SYNCHRONOUS_CALL(semPrintf, putLine(line));

	// moves on to next position
        i++;
        if (i == NBMAX_BUFFER) i = 0;
	
	//broadcast that position is free for reading to all readers
	for(int i =0; i<NBLECTEURS; i++) CHECK(sem_post(&(buf.semBufferPlein[i])), "sem_post() sur semBufferPlein");
    }
    pthread_exit(0);
}
/* ------------------------------------------------------------------------ */
void  lecteur(long no)
{
    int i = 0, n = 0;
    char line[MAXLEN + 1];
    SYNCHRONOUS_CALL(
	semPrintf,
    	printf("Lancement du lecteur %ld d'id   = 0x%08lx\n",no, (long)pthread_self());
    	fflush(stdout)
    );

    while ( n != OVER ) {
       tempo(10, 100);
       // wait for a position to be free to be read
	CHECK(sem_wait(&(buf.semBufferPlein[no])),"sem_wait() sur semBufferPlein");
	
	// read a position from the buffer
        n = buf.buffer[i];
        sprintf(line,"%*sLe lecteur n°%ld lit %d dans le buffer %d\n",
                5 * (int)(no + 1), "   ", no,   n, i);
        SYNCHRONOUS_CALL(semPrintf, putLine(line));

	// updates the nReads vector to mark the buffer position has been read
	SYNCHRONOUS_CALL(buf.semAcessReads[i],buf.nReads[i]++);

	//check if it was the last to read this position
	if(buf.nReads[i] == NBLECTEURS){
		CHECK(sem_post(&(buf.semBufferVide)), "sem_post() sur semBufferVide");
	}

	// go to next position	
        i++;
        if (i == NBMAX_BUFFER) i = 0;
	
    }
    pthread_exit(0);
}

/* ------------------------------------------------------------------------ */
void init(circBuffer_t * b)
{
    int i;
    for (i = 0; i < NBMAX_BUFFER; i++){
        b->buffer[i] = OVER;
    	b->nReads[i] = 0;
    	CHECK(sem_init(&(b->semAcessReads[i]),0,1),"sem_init() sur semAcessReads");
    }

    CHECK(sem_init(&(b->semBufferVide),0,NBMAX_BUFFER),"sem_init() sur semBufferVide");
    for(int i=0;i<NBLECTEURS; i++) CHECK(sem_init(&(b->semBufferPlein[i]),0,0), "sem_init sur semBufferPlein");
    /*  ............................ à compléter  ......................... */
}

/* ------------------------------------------------------------------------ */
void tempo(int min, int max)
{

    struct timespec delai = { .tv_sec = 0, .tv_nsec = 0};
    delai.tv_nsec = TEMPO_1ms * (min + rand() % (max - min + 1));
    nanosleep(&delai, NULL);
}

/* ------------------------------------------------------------------------ */
void putLine (const char *s)
{
    for ( ; *s ==  ' '; s++) printf("%c", *s);
    fflush(stdout);
    for ( ; *s != '\0'; s++) {
        tempo(0, 100);
        printf("%c", *s);
        fflush(stdout);
    }
}
/* ------------------------------------------------------------------------ */
