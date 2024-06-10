nectionStatus ){
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
	newAction.sa_flags = 0;
	
	sigemptyset(&newAction.sa_mask);
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
	//wait for child to end
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


