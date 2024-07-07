# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <errno.h>
# include <unistd.h>

#define CHECK(sts , msg) \
if (-1 == (sts)) { \
perror (msg); \
exit( EXIT_FAILURE ); \
}

const int sequence[] = {SIGINT,SIGTSTP, SIGINT, SIGTSTP};
int current_state = 0;

static void signalHandler (int);

int main (int argc , char *argv [])
{
     struct sigaction newAction ;
    /* Initialisation de la structure sigaction */
    newAction.sa_handler = signalHandler ;
    CHECK( sigemptyset (& newAction . sa_mask ), " sigemptyset ()");
    newAction . sa_flags = 0;
    /* Installation du gestionnaire du signal SIGINT */
    CHECK( sigaction (SIGINT , &newAction , NULL), " sigaction ()");
    CHECK( sigaction (SIGTSTP, &newAction , NULL), " sigaction ()");

    while(current_state != 4) pause();
    printf("\n");
    exit( EXIT_SUCCESS );
}
    
static void signalHandler (int numSig )
    {
    if(numSig == sequence[current_state]){
    	current_state+=1;
    }else {
    	current_state =0;
    }
}
