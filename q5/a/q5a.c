#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

void waitSem(int *semaphore, int n_listener) {
    while (*semaphore < n_listener);
    *semaphore -= n_listener;
    // printf("Semaphore: %d\n", *semaphore);
}

void signalSem(int *semaphore) {
    (*semaphore)++;
    // printf("Semaphore: %d\n", *semaphore);
}

int main(void) {
    int *semaphore;
    int shmid = shmget(1234, sizeof(int), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    semaphore = shmat(shmid, NULL, 0);
    if (semaphore == (void *)-1) {
        perror("shmat");
        exit(1);
    }
    *semaphore = 0;

    int n_listeners;
    printf("Enter number of listeners:");
    scanf("%d", &n_listeners);

    int fd[2 * n_listeners];
    int i;
    for (i = 0; i < n_listeners; ++i) {
        pipe(&fd[2 * i]);
    }
    for (i = 0; i < n_listeners; ++i) {
        printf("PARENT : For listener %d : Read from %d & write to %d\n", i, fd[2 * i], fd[2 * i + 1]);
    }

    pid_t pids[n_listeners];
    for (i = 0; i < n_listeners; ++i) {
        if ((pids[i] = fork()) == 0) {
            printf("CHILD: Inside child %d, Reading from %d\n", i, fd[2 * i]);
            close(fd[2 * i + 1]);   // close the writing end of the pipe in the child process
            int temp;
            int bytesRead;
            while (1) {
                if ((bytesRead = read(fd[2 * i], &temp, sizeof(int))) > 0) {
                    signalSem(semaphore);
                    if (temp < -273) {
                        exit(0);
                    }
                    printf("CHILD %d: Temperature (in degree Celsius): %d\n", i, temp);
                }
            }
        } else if (pids[i] < 0) {
            perror("fork");
            exit(1);
        }
    }

    while (1) {
        for (i = 0; i < n_listeners; ++i) {
            close(fd[2 * i]);   // close the reading end of the pipe for parent process;
        }
        int temp;
        printf("PARENT: Enter temperature (in degree Celsius): \n");
        scanf ("%d", &temp);
        for (i = 0; i < n_listeners; ++i) {
            write(fd[2 * i + 1], &temp, sizeof(int));
        }
        waitSem(semaphore, n_listeners);
        if (temp < -273) {
            break;
        }
    }

    wait(NULL);

    shmdt(semaphore);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}