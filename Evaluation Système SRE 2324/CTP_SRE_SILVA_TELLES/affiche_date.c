# include <stdio.h>
# include <stdlib.h>
# include <signal.h>
# include <errno.h>
# include <unistd.h>
# include <sys/wait.h>

#define CHECK(sts , msg) \
if (-1 == (sts)) { \
perror (msg); \
exit( EXIT_FAILURE ); \
}

#define STOP_TIME 3

#define NORMAL_STATE 0
#define PREPARE_QUIT_STATE 1
#define QUIT_STATE 2

static void signalHandler (int);
const char * cmd [] = {"date", (char *) 0};
int state = 0;

int main (int argc , char *argv [])
{
     struct sigaction newAction ;

    /* Initialisation de la structure sigaction */
    newAction . sa_handler = signalHandler ;
    CHECK( sigemptyset (& newAction . sa_mask ), " sigemptyset ()");
    newAction . sa_flags = SA_RESTART;
    /* Installation du gestionnaire du signal SIGINT */
    CHECK( sigaction (SIGINT , &newAction , NULL), " sigaction ()");
    CHECK( sigaction (SIGALRM , &newAction , NULL), " sigaction ()");
  
    pid_t pidFils;
    while(state != QUIT_STATE){
	if( state == NORMAL_STATE){	
		pidFils = fork();
		if(pidFils == 0)
			execv("/usr/bin/date",(char**)cmd);
		wait(NULL);
		alarm(1);	
	}
	pause();
	
    }

    printf("\n");
    exit( EXIT_SUCCESS );
}
    
static void signalHandler (int numSig )
    {
    switch (numSig ) {
        case SIGINT : /* traitement de SIGINT */
	    if(state == NORMAL_STATE){
            	printf ("pour quitter, entrez à nouveau Ctrl+C dans moins de trois secondes\n");
		fflush(stdout);
		state = PREPARE_QUIT_STATE;
	    	alarm(STOP_TIME);
	    } else if(state == PREPARE_QUIT_STATE){
	    	state = QUIT_STATE;
	    }
            break;
        case SIGALRM : /* traitement de SIGALRM */
	    state = NORMAL_STATE;
            break;
    }
}
