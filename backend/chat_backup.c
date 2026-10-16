#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define FIFO_ONE "/tmp/ipc_chat_one"
#define FIFO_TWO "/tmp/ipc_chat_two"
#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    char message[BUFFER_SIZE];
    char received[BUFFER_SIZE];

    if (argc != 2 ||
        (strcmp(argv[1], "1") != 0 &&
         strcmp(argv[1], "2") != 0)) {
       fprintf(stderr, "Usage: %s 1 OR %s 2\n", argv[0], argv[0]);
        return 1;
    }

    const char *out_fifo =
        strcmp(argv[1], "1") == 0 ? FIFO_ONE : FIFO_TWO;
    const char *in_fifo =
        strcmp(argv[1], "1") == 0 ? FIFO_TWO : FIFO_ONE;

    if (mkfifo(FIFO_ONE, 0600) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    if (mkfifo(FIFO_TWO, 0600) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    printf("IPC Chat - User %s\n", argv[1]);
    printf("Start the other user in another terminal.\n");
    printf("Type /quit to exit.\n");

    int in_fd = open(in_fifo, O_RDWR);
    if (in_fd == -1) {
        perror("open incoming FIFO");
        return 1;
    }

    int out_fd = open(out_fifo, O_RDWR);
    if (out_fd == -1) {
        perror("open outgoing FIFO");
        close(in_fd);
        return 1;
    }

    printf("Connected! You can chat now.\n");

    while (1) {
        printf("You: ");
        fflush(stdout);

        if (fgets(message, sizeof(message), stdin) == NULL)
            break;

        message[strcspn(message, "\n")] = '\0';

        if (strcmp(message, "/quit") == 0)
            break;

        if (write(out_fd, message, strlen(message) + 1) == -1) {
            perror("write");
            break;
        }

        ssize_t n = read(in_fd, received, sizeof(received) - 1);
        if (n <= 0) {
            printf("Other user disconnected.\n");
            break;
        }

        received[n] = '\0';
        printf("Other user: %s\n", received);
    }

    close(in_fd);
    close(out_fd);
    printf("Chat ended.\n");
    return 0;
}
