#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

typedef struct factorial_thread_data{
	long input;
	long res;
}factorial_thread_data;

long factorial(long n){
	if(n==0) return 1;
	return n*factorial(n-1);
}

void* thread_function(void* raw_data){
	factorial_thread_data* data = (factorial_thread_data*)raw_data;
	data->res = factorial(data->input);
	pthread_exit(0);
	return NULL;
}

int main(int argc, char** argv){
	int len = argc;	
	factorial_thread_data* threads = (factorial_thread_data*) malloc(len*sizeof(factorial_thread_data));
	pthread_t* tid = (pthread_t*)malloc(len*sizeof(factorial_thread_data)); 

	for(int i=0; i< len; i++){
		threads[i].input = atoi(argv[i]);
		pthread_create(&tid[i],NULL,thread_function,(void*)&threads[i] );
	}

	for(int i=0;i<len;i++){
		long res;
		pthread_join(tid[i],(void*)&res);
		printf("the factorial of %ld is %ld\n",threads[i].input,threads[i].res);
	}
}
