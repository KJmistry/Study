#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define QUEUE_KEY 1234
#define MAX_TEXT  100

struct message
{
    long msg_type;            // Message type
    char msg_text[MAX_TEXT];  // Message text
};

int msg_id;

// Signal handler for cleanup
void cleanup(int signum)
{
    printf("\nReceiver terminating. Cleaning up...\n");
    if (msgctl(msg_id, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
    }
    exit(0);
}

int main()
{
    // Set up signal handler
    signal(SIGINT, cleanup);

    // Access the message queue
    msg_id = msgget(QUEUE_KEY, 0666);
    if (msg_id == -1)
    {
        perror("msgget");
        exit(1);
    }

    printf("Receiver started. Waiting for messages.\n");

    while (1)
    {
        struct message msg;

        // Receive a message of type 1
        if (msgrcv(msg_id, &msg, sizeof(msg.msg_text), 1, 0) == -1)
        {
            perror("msgrcv");
        }
        else
        {
            printf("Message received: %s\n", msg.msg_text);
        }
    }

    return 0;
}
