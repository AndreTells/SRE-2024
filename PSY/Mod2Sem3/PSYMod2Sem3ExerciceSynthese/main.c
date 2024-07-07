#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

#define MAXLEN 8
#define MAX_TIME 30//30 s is the max time to enter a password
#define MAX_NUM_ATTEMPTS 3

#define TRUE 1
#define FALSE 0

#define PASSWORD "1234"
const char* AttemptMessages[]=  {"Premier essai .... : ", "2ème essai ....... : ", "Dernier essai .... : "};
pid_t childPid;

int TryGetPassword(void){
	char password[MAXLEN+1]; 
	char *p;
	fgets(password, MAXLEN, stdin);
	
	// clean the str input by the user
	if((p=strchr(password,'\n'))!= NULL) *p = '\0';
	else while (getchar() != '\n');
	
	//check that the password is correct
	if(strcmp(password,PASSWORD) == 0) return TRUE;
	return FALSE;
}

int RepeatedTryGetPassword(int numAttempts){
	for(int i=0; i<numAttempts; i++){
		printf("%s", AttemptMessages[i]);
		if(TryGetPassword()) return TRUE;
	}
	return FALSE;	
}

void PrintChildEnd(char* reason, char* conectionStatus ){
	// Ends the child process
	printf("Terminaison du processus %d	%s : %s\n", childPid, reason, conectionStatus);
}

static void SignalHandler(pid_t numSig){
	switch(numSig){
		case SIGALRM:
			kill(childPid, SIGUSR2);
		break;

		case SIGINT:
			printf("[%d] --> Le Ctrl-c est désactivé.\n",getpid());
		break;
	}
}

void SetupSignalHandler(){
	struct sigaction newAction;	
	newAction.sa_handler = SignalHandler; // sets SignalHandler as the function that deals with incoming signals for this sigAction object
	newAction.sa_flags = SA_RESTART;
	
	//register necessary signals
	sigaction(SIGINT, &newAction, NULL);
	sigaction(SIGALRM, &newAction, NULL);
}

int ChildProcessFunction(){
	//mask the alarm signal
	sigset_t newMask;
	sigemptyset(&newMask);
	sigaddset(&newMask,SIGALRM);	
	sigprocmask(SIG_BLOCK,&newMask,NULL);
	
	// execute its task
	return RepeatedTryGetPassword(MAX_NUM_ATTEMPTS); 
}

int main(void){
	SetupSignalHandler();
	//print start message
	printf("Vous avez 30 secondes pour entrer votre mot de passe\n");
	//capture interrupts
	//create child process
	childPid = fork();

	if(childPid== 0) return ChildProcessFunction(); // execute on child process
	// mask the SIGINT signal
	sigset_t newMask;
	sigemptyset(&newMask);
	sigaddset(&newMask,SIGINT);	
	sigprocmask(SIG_BLOCK,&newMask,NULL);
	//set up alarm to go off at excatly MAX_TIME 
	alarm(MAX_TIME);
	int status=0;
	int waitRes= wait(&status);
	printf("\n\n");
	// checks if wait returned failure or not
	if(waitRes == -1){
		perror("error");
		exit(-1);
	}
	if(WIFEXITED(status)){
	       if(WEXITSTATUS(status)==TRUE) PrintChildEnd((char*)"Mot de passe valide",(char*)"connexion acceptée");
	       else  PrintChildEnd((char*)"Échec des 3 tentatives",(char*)"connexion refusée");
	}
	else if(WIFSIGNALED(status) && WTERMSIG(status) == SIGUSR2) {
			PrintChildEnd((char*)"Délai expiré",(char*)"connexion refusée");	
	}
	//check which was the signal received
	//StopGivingPasswordAttempts();
	printf("[%d] Fin du processus père\n", getpid());
	return 0;
}
