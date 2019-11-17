#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_sender2this "ConversationChannel1"
#define FIFO_this2sender "ConversationChannel2"
#define MSG_LEN 500

int main(void) {
    char msg2send[MSG_LEN], msg2recieve[MSG_LEN];

    mknod(FIFO_sender2this, S_IFIFO | 0666, 0);
    mknod(FIFO_this2sender, S_IFIFO | 0666, 0);

    printf("Reciever: Opening ConversationChannel1...\n");
    int sender2this = open(FIFO_sender2this, O_RDONLY);
    printf("Reciever: Opening ConversationChannel2...\n");
    int this2sender = open(FIFO_this2sender, O_WRONLY);

    if (!fork()) {
        // printf("Inside Reciever Child\n");
        int num;
        while (1) {
            // scanf("%s", &msg2send);
            strcpy(msg2send, "\0");
            fgets(msg2send, MSG_LEN, stdin);
            num == write(this2sender, msg2send, strlen(msg2send));
            if (num == -1) {
                perror("write");
            } else {
                printf("<== Sending: %s\n", msg2send);
            }
        }
    } else {
        // printf("Inside Reciever Parent\n");
        int num;
        do {
            num = read(sender2this, msg2recieve, MSG_LEN);
            if (num == -1) {
                perror("read");
            } else {
                printf("==> Received: %s\n", msg2recieve);
            }
        } while (num > 0);
    }
}