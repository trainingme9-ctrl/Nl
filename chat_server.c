#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0), n, opt = 1;
    if (s < 0) return 1;

    struct sockaddr_in a;
    memset(&a, 0, sizeof(a));
    a.sin_family = AF_INET;
    a.sin_port = htons(8081);
    a.sin_addr.s_addr = INADDR_ANY;

    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (bind(s, (struct sockaddr *)&a, sizeof(a)) < 0) return 1;
    
    listen(s, 3);
    printf("Server live on port 8081. Waiting for connection...\n");

    n = accept(s, NULL, NULL); 
    if (n < 0) return 1;
    
    printf("Client connected! Type 'exit' to quit.\n");
    char b[1024];

    pid_t pid = fork();
    if (pid == 0) {
        // CHILD PROCESS: Only listens for incoming messages from the client
        while (1) {
            memset(b, 0, sizeof(b));
            if (read(n, b, sizeof(b)) <= 0 || !strncmp(b, "exit\n", 5)) {
                printf("\n[Client disconnected]\n");
                break;
            }
            printf("Client: %s", b);
        }
        exit(0);
    } 
    else {
        // PARENT PROCESS: Only waits for keyboard input to send
        while (1) {
            memset(b, 0, sizeof(b));
            fgets(b, sizeof(b), stdin);
            send(n, b, strlen(b), 0);
            if (!strncmp(b, "exit\n", 5)) 
                break;
        }
    }

    close(n); 
    close(s);
    return 0;
}