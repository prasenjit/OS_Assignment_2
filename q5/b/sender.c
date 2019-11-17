#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_this2reciever "ConversationChannel1"
#define FIFO_reciever2this "ConversationChannel2"
#define MSG_LEN 500

int main(void) {
    char msg2send[MSG_LEN], msg2recieve[MSG_LEN];

    mknod(FIFO_this2reciever, S_IFIFO | 0666, 0);
    mknod(FIFO_reciever2this, S_IFIFO | 0666, 0);

    printf("Sender: Opening ConversationChannel1...\n");
    int this2receiver = open(FIFO_this2reciever, O_WRONLY);
    printf("Sender: Opening ConversationChannel2...\n");
    int reciever2this = open(FIFO_reciever2this, O_RDONLY);

    if (!fork()) {
        // printf("Inside Sender Child\n");
        int num;
        while (1) {
            // scanf("%s", &msg2send);
            strcpy(msg2send, "\0");
            fgets(msg2send, MSG_LEN, stdin);
            num == write(this2receiver, msg2send, strlen(msg2send));
            if (num == -1) {
                perror("write");
            } else {
                printf("<== Sending: %s\n", msg2send);
            }
        }
    } else {
        // printf("Inside Sender Parent\n");
        int num;
        do {
            num = read(reciever2this, msg2recieve, MSG_LEN);
            if (num == -1) {
                perror("read");
            } else {
                printf("==> Received: %s\n", msg2recieve);
            }
        } while (num > 0);
    }
    
    return 0;
}