#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include <semaphore.h>

#define N 5
#define EATING_TIME 3
#define THINKING_TIME 2 

#define RIGHT_FORK(phiId) (phiId+1)% N
#define LEFT_FORK(phiId) phiId

#define TAKE_FORK(forkId) sem_wait(&(tableFork[forkId]));	
#define RELEASE_FORK(forkId) sem_post(&(tableFork[forkId]));	
#define SYNCHRONOUS_CALL(sem,call) sem_wait(&sem);call; sem_post(&sem)

sem_t semTryEat;
sem_t tableFork[N];
sem_t semPrint;

enum state{
	THINKING = 0,
	HUNGRY = 1,
	EATING = 2,
        WAITING = 3,
	NONE = 4
}state;


enum state philosopher_state[N];


void stopEat(int phiId);
void tryEat(int phiId);
void think(int phiId);
void eat(int phiId);
void * philosopher(void * param);


int main(void){
	sem_init(&semTryEat, 0, N-1);
	sem_init(&semPrint, 0 , 1);
	for(int i=0; i<N; i++) sem_init(&(tableFork[i]), 0, 1);
	// create thread for each philosopher
	pthread_t tid[N];	
	for(int i =0; i<N; i++) pthread_create(&(tid[i]), NULL, philosopher, (void *)((long) i));

	//make sure all threads are done before code finishes
	for(int i =0; i<N; i++) pthread_join(tid[i],NULL);

	//safely destroy all semaphores
	sem_destroy(&semTryEat);
	for(int i=0; i<N; i++) sem_destroy(&(tableFork[i]));
	return 0;
}

void stopEat(int phiId){
	RELEASE_FORK(RIGHT_FORK(phiId));
	RELEASE_FORK(LEFT_FORK(phiId));
	sem_post(&semTryEat);

}

void tryEat(int phiId){
	SYNCHRONOUS_CALL(semPrint,printf("philosopher %d is waiting to eat\n", phiId);fflush(stdout));
	philosopher_state[phiId] = WAITING;
	sem_wait(&semTryEat);	
	TAKE_FORK(RIGHT_FORK(phiId));
	TAKE_FORK(LEFT_FORK(phiId));
	philosopher_state[phiId] = NONE;
}

void think(int phiId){
	philosopher_state[phiId] = THINKING;
	SYNCHRONOUS_CALL(semPrint,printf("philosopher %d is thinking\n", phiId);fflush(stdout));
	sleep(THINKING_TIME);
	philosopher_state[phiId] = HUNGRY;
}

void eat(int phiId){
	philosopher_state[phiId] = EATING;
	SYNCHRONOUS_CALL(semPrint,printf("philosopher %d is eating\n", phiId);fflush(stdout));
	sleep(EATING_TIME);
	philosopher_state[phiId] = NONE;
}


void* philosopher(void* param){
	long phiId = (long) param;
	SYNCHRONOUS_CALL(semPrint,printf("philosopher %ld initialised \n", phiId);fflush(stdout));
	while(1){
		think(phiId); //think
		tryEat(phiId);
		eat(phiId); 
		stopEat(phiId);
	}
	pthread_exit(0);
}
