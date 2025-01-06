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
    printf("\nSender terminating. Cleaning up...\n");
    if (msgctl(msg_id, IPC_RMID, NULL) == -1)
    {
        perror("msgctl");
    }
    else
    {
        printf("Message queue removed.\n");
    }
    exit(0);
}

int main()
{
    // Set up signal handler
    signal(SIGINT, cleanup);

    // Create or access the message queue
    msg_id = msgget(QUEUE_KEY, IPC_CREAT | 0666);
    if (msg_id == -1)
    {
        perror("msgget");
        exit(1);
    }

    printf("Sender started. Enter messages to send.\n");

    while (1)
    {
        struct message msg;
        msg.msg_type = 1;  // Default message type

        printf("Enter message: ");
        fgets(msg.msg_text, MAX_TEXT, stdin);
        msg.msg_text[strcspn(msg.msg_text, "\n")] = '\0';  // Remove newline

        // Send the message
        if (msgsnd(msg_id, &msg, sizeof(msg.msg_text), 0) == -1)
        {
            perror("msgsnd");
        }
        else
        {
            printf("Message sent: %s\n", msg.msg_text);
        }
    }

    return 0;
}
