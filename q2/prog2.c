#include<stdio.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

#define X_SLEEP_DURATION 1
#define Y_SLEEP_DURATION 2

void  waitM(int *semaphore){
	while(*semaphore <= 0);
	(*semaphore)--;
}

void signalM(int *semaphore){
	(*semaphore)++;
}

void create_two(int *semaphore){
	pid_t t1 = fork();
	if (t1 == 0) {	// child_X of create_two
		for(int i=0;i<10;i++)
		{
			printf("\t\t | CHILD_X  iter:%d\n",i);
			sleep(X_SLEEP_DURATION);
			signalM(semaphore);
		}
	}
	else {	// parent process ==> create_two
		pid_t t2=fork();
		int childs;
		if (t2 == 0) {	// child_Y of create_two
			for(int i=0;i<10;i++) {
				waitM(semaphore);
				printf("CHILD_Y iter: %d  | \n",i);
				sleep(Y_SLEEP_DURATION);
			}
		}
		else {
			pid_t tpid;
			do {
				tpid=wait(&childs);	// wait for child_X and child_Y
			} while (tpid != t2);		
		}
	}
}

int main() {
	int *semaphore;
	int shmid;
	shmid = shmget(241, sizeof(int), 0666 | IPC_CREAT);
	if (shmid == -1) {
		perror("Shared Memory");
		return 1;
	}
	semaphore = shmat(shmid, NULL, 0);
	if (semaphore == (void *)-1) {
		perror("Shared Memory Attach");
		return 1;
	}
	printf("SEMAPHORE!!!!\n");
	create_two(semaphore);
	shmdt(semaphore);
	shmctl(shmid, IPC_RMID, NULL);
	return(0);
}