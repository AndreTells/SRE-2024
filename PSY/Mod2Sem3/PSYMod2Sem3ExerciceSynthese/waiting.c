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
