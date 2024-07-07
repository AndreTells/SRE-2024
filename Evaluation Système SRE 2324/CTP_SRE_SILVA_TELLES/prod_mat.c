#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <time.h>
#include <sched.h>

#define MAX 10000
#define DIM 2
int A[DIM][DIM];
int B[DIM][DIM];
int C[DIM][DIM];

typedef struct pos_t{
	int i;
	int j;
	long res;
}pos_t;

pos_t data_threads[DIM][DIM];
pthread_t pid[DIM][DIM];


void* calc_pos(void* data);

int main (int argc , char *argv [])
{
	for(int i=0;i<DIM;i++) for(int j=0; j<DIM; j++) A[i][j] = rand()%MAX;
	for(int i=0;i<DIM;i++) for(int j=0; j<DIM; j++) B[i][j] = rand()%MAX;

	for(int i=0;i<DIM;i++) for(int j=0; j<DIM; j++) {
		data_threads[i][j].i = i;
		data_threads[i][j].j = j;
		pthread_create(&(pid[i][j]),NULL,calc_pos,(void*) &(data_threads[i][j])); 
	}


	for(int i=0;i<DIM;i++){
	       	for(int j=0; j<DIM; j++){
		pthread_join(pid[i][j],NULL);
		C[i][j] = data_threads[i][j].res;
		printf("%d ", C[i][j]);
	}
		printf("\n");
	}
}

void* calc_pos(void* data){
	pos_t* pos_data = (pos_t*) data;
	pos_data->res = 0; 
	for (int i=0; i< DIM; i++){
		pos_data->res += A[pos_data->i][i]*B[i][pos_data->j];
	}
	pthread_exit(0);
}
