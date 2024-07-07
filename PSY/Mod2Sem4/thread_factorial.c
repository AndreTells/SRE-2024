#include<semaphore.h>
#include <errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

#define CHECK_CALL(sts, msg) if (-1 == (sts) && errno != 0) 		\
	{	perror(msg); exit(EXIT_FAILURE); }

#define CHECK_PTR(ptr, msg) if(ptr == NULL) 				\
	{ perror(msg); exit(EXIT_FAILURE);}

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
	int len = argc -1;	
	factorial_thread_data* thread_data = (factorial_thread_data*) malloc(len*sizeof(factorial_thread_data));
	CHECK_PTR(thread_data, "malloc for factorial_thread_data list returned false");
	pthread_t* tid = (pthread_t*)malloc(len*sizeof(factorial_thread_data)); 
	CHECK_PTR(tid, "malloc for pthread_t list returned false");

	for(int i=0; i< len; i++){
		thread_data[i].input = atoi(argv[i+1]);
		CHECK_CALL(pthread_create(&tid[i],NULL,thread_function,(void*)&thread_data[i]), "pthread create failed");
	}

	for(int i=0;i<len;i++){
		int res;
		CHECK_CALL(pthread_join(tid[i],(void**)&res),"pthread join failed");
		CHECK_CALL(res,"thread_function failed");
		printf("the factorial of %ld is %ld\n",thread_data[i].input,thread_data[i].res);
	}
}
