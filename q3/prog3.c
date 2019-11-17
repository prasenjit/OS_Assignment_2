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
#include<time.h>

typedef struct shared_buffer{
	int arr[25];
	int pid[25];
	int aStart;
	int aEnd;
	int total;
} ShBuf;

void driver(ShBuf *samp,int p,int c){
	pid_t pr = fork();
	if (pr == 0) {	// child process of driver ==> parent process for all producers
		for(int i = 0; i < p; i++) {	// fork all producers
			pid_t t1 = fork();
			if(t1 == 0){
				srand(getpid());
				int temp = (rand() % 80) + 1;
				samp->arr[samp->aEnd%25] = temp;
				samp->pid[samp->aEnd%25] = getpid();
				samp->aEnd = (samp->aEnd + 1) % 25;;
				exit(0);
			}
			else {	// wait for all producers to finish
				pid_t t;
				int ch;
				do {
					t = wait(&ch);
				} while (t != t1);
			}	
		}
		exit(0);
	}
	else {	// parent process ==> driver
		pid_t co = fork();
		if (co == 0) {
			for(int i = 0; i < c; i++){
				pid_t t2 = fork();	// fork all consumers
				if(t2 == 0){
					while (samp->arr[samp->aStart] == 0);
					samp->total = samp->total + samp->arr[samp->aStart];
					samp->aStart = (samp->aStart + 1) % 25;;
					exit(0);
				}	
				else {
					pid_t t;
					int ch;
					do {
						t = wait(&ch);
					} while (t != t2);
				}
			}
			exit(0);
		}	
		else {	// wait for all consumers to finish
			pid_t tpid;
			int childs;
			do {
				tpid = wait(&childs);
			} while(tpid != co);
		}
	}
}

int main() {
	int prod = 1, cons = 1;
	time_t t;
	srand(t);
	do {
		printf("Number of Producers must be greater than or equal to Number of Consumers.\n");
		printf("No. of Producers: ");
		scanf("%d", &prod);
		printf("No. of Consumers: ");
		scanf("%d", &cons);
	} while (prod < cons);
	ShBuf *sample;
	int shmid_samp = shmget(262, sizeof(ShBuf), 0666 | IPC_CREAT);
	if(shmid_samp == -1) {
		perror("Shared Memory");
		return 1;
	}
	sample = shmat(shmid_samp, NULL, 0);
	if(sample == (void*)-1) {
		perror("Shared Memory Attach");
		return 1;
	}
	driver(sample, prod, cons);
	for(int i = 0; i < sample->aEnd; i++) {
		printf("\narr[%d]: %d", i, sample->arr[i]);
		printf("\tpid[%d]: %d", i, sample->pid[i]);
	}
	printf("\nTotal : %d\n", sample->total);
	shmdt(sample);
	shmctl(shmid_samp, IPC_RMID,NULL);
	return 0;
}
