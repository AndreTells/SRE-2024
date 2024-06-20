/* ------------------------------------------------------------------------ */
/*                   E N T Ê T E S    S T A N D A R D S                     */
/* ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#include <unistd.h>
#include <pthread.h>

/* ------------------------------------------------------------------------ */
/*              C O N S T A N T E S     S Y M B O L I Q U E S               */
/* ------------------------------------------------------------------------ */
#define NBMAX_THREADS   2
#define TEMPO_10ms      10000000L

/* ------------------------------------------------------------------------ */
/*              D É F I N I T I O N S   D E   T Y P E S                     */
/* ------------------------------------------------------------------------ */
typedef void * (*pf_t)(void *);

/* ------------------------------------------------------------------------ */
/*                      M A C R O - F O N C T I O N S                       */
/* ------------------------------------------------------------------------ */
#define CHECK_T(status, msg)                        \
  if (0 != (status))   {                            \
    fprintf(stderr, "pthread erreur : %s\n", msg);  \
    exit (EXIT_FAILURE);                            \
  }

/* ------------------------------------------------------------------------ */
/*            P R O T O T Y P E S    D E    F O N C T I O N S               */
/* ------------------------------------------------------------------------ */
void monThread (long no);
void bye(void);

/* ------------------------------------------------------------------------ */
/*                 V A R I A B L E S    G L O B A L E S                     */
/* ------------------------------------------------------------------------ */
const char *msgPi ="Que J'aime A Faire Apprendre Un NOMBRE Utile Aux SAGES";
/* ------------------------------------------------------------------------ */
int main(int argc, char *argv[])
{
    pthread_t tid[NBMAX_THREADS];
    long i;
    double * status;

    srand(time(NULL));  /* Init. du générateur de nombres pseudo-aléatoires */
    atexit(bye);
    printf("Le processus (pid = %d, tid = 0x%lx) va créer %d threads\n",
           getpid(), (long)pthread_self(), NBMAX_THREADS);
    /* Création des NBMAX_THREADS                                           */
    for (i = 0; i < NBMAX_THREADS; i++)
        CHECK_T(pthread_create (&tid[i], NULL, (pf_t)monThread,
                                (void *)i), "pthread_create()");

    printf("Attente de la terminaison des %d threads\n\n", NBMAX_THREADS);
    for (i = 0; i < NBMAX_THREADS; i++) {
        CHECK_T(pthread_join (tid[i], (void **) & status),"pthread_join()");
        printf("\nFin du thread n°%ld avec le code %.10f\n", i + 1, *status);
        free(status);
    }
    return EXIT_SUCCESS;
}

/* ------------------------------------------------------------------------ */
void monThread (long  no)
{
    double * retVal = (double *) calloc (1, sizeof(double));
    struct timespec delai = { .tv_sec = 0, .tv_nsec = 0};
    long n = 0;
    double diviseur = 1.;
    const char *p;
    /*  Attente initiale du thread                                          */
    delai.tv_nsec = TEMPO_10ms * (10 + rand() % 31);
    nanosleep(&delai, NULL);

    for (p = msgPi; *p != '\0'; p++) {
        if (isalpha(*p)) {
            n++;
            if (no == 1)
                printf("%c", tolower(*p));
            else
                printf("%c", toupper(*p));
        }
        else {
            if (n > 0) {
                *retVal += n / diviseur;
                n = 0;
                diviseur *= 10.;
            }
            printf("%c", *p);
            fflush(stdout);
        }
        /* Attente entre le traitement de chaque caractère                  */
        delai.tv_nsec = TEMPO_10ms * (5 + rand() % 16);
        nanosleep(&delai, NULL);
    }
    /* traitement du dernier mot                                            */
    if (n > 0) *retVal += n / diviseur;

    /* terminaison du thread                                                */
    pthread_exit((void *) retVal);
}

/* ------------------------------------------------------------------------ */
void bye (void)
{
    printf("Fin du processus %d (tid = 0x%lx)\n",
           getpid(), pthread_self());
}
/* ------------------------------------------------------------------------ */

