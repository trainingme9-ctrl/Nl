#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return 1;

    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons(8081);

    if (inet_pton(AF_INET, "127.0.0.1", &a.sin_addr) <= 0) return 1;
    if (connect(s, (struct sockaddr *)&a, sizeof(a)) < 0) return 1;

    printf("Connected. Type 'exit' to quit.\n");
    char b[1024];

    pid_t pid = fork();

    if (pid == 0) {
        // CHILD PROCESS: Only listens for incoming messages from the server
        while (1) {
            memset(b, 0, sizeof(b));
            if (read(s, b, sizeof(b)) <= 0 || !strncmp(b, "exit\n", 5)) {
                printf("\n[Server disconnected]\n");
                break;
            }
            printf("Server: %s", b);
        }
        exit(0);
    } 
    else {
        // PARENT PROCESS: Only waits for keyboard input to send
        while (1) {
            memset(b, 0, sizeof(b));
            fgets(b, sizeof(b), stdin);
            send(s, b, strlen(b), 0);
            if (!strncmp(b, "exit\n", 5)) 
                break;
        }
    }

    close(s);
    return 0;
}