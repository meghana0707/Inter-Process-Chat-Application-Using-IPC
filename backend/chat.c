#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>

#define FIFO_ONE "/tmp/ipc_chat_one"
#define FIFO_TWO "/tmp/ipc_chat_two"
#define BUFFER_SIZE 1024

static int in_fd, out_fd;
static volatile int running = 1;

void *receive_messages(void *arg) {
    (void)arg;
    char received[BUFFER_SIZE];

    while (running) {
        ssize_t n = read(in_fd, received, sizeof(received) - 1);
        if (n > 0) {
            received[n] = '\0';
            printf("\nOther user: %s\nYou: ", received);
            fflush(stdout);
        } else if (n < 0 && errno != EINTR) {
            perror("read");
            break;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2 ||
        (strcmp(argv[1], "1") != 0 && strcmp(argv[1], "2") != 0)) {
        fprintf(stderr, "Usage: %s 1 OR %s 2\n", argv[0], argv[0]);
        return 1;
    }

    const char *out_fifo = strcmp(argv[1], "1") == 0 ? FIFO_ONE : FIFO_TWO;
    const char *in_fifo = strcmp(argv[1], "1") == 0 ? FIFO_TWO : FIFO_ONE;

    if (mkfifo(FIFO_ONE, 0600) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }
    if (mkfifo(FIFO_TWO, 0600) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    in_fd = open(in_fifo, O_RDWR);
    if (in_fd == -1) {
        perror("open incoming FIFO");
        return 1;
    }

    out_fd = open(out_fifo, O_RDWR);
    if (out_fd == -1) {
        perror("open outgoing FIFO");
        close(in_fd);
        return 1;
    }

    pthread_t receiver;
    if (pthread_create(&receiver, NULL, receive_messages, NULL) != 0) {
        perror("pthread_create");
        close(in_fd);
        close(out_fd);
        return 1;
    }

    printf("IPC Chat - User %s\n", argv[1]);
    printf("Connected! You can chat now.\n");
    printf("Type /quit to exit.\n");

    char message[BUFFER_SIZE];
    while (running) {
        printf("You: ");
        fflush(stdout);

        if (fgets(message, sizeof(message), stdin) == NULL)
            break;

        message[strcspn(message, "\n")] = '\0';
        if (strcmp(message, "/quit") == 0)
            break;
        if (message[0] == '\0')
            continue;

        if (write(out_fd, message, strlen(message) + 1) == -1) {
            perror("write");
            break;
        }
    }

    running = 0;
    close(in_fd);
    close(out_fd);
    pthread_cancel(receiver);
    pthread_join(receiver, NULL);
    printf("Chat ended.\n");
    return 0;
}
