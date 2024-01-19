#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define TCP_SEND_RETRIES 5

void tcp_connection(char* msg) {
    int socket_fd;
    struct sockaddr_in server_addr;
    char client_message[2000];
    int attempts = 0;
    
    while (attempts < TCP_SEND_RETRIES) {
        attempts++;
        memset(client_message, '\0', sizeof(client_message));

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            printf("Failed to create socket\n");
            continue;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(2000);
        server_addr.sin_addr.s_addr = inet_addr("192.168.0.131");
        int err = connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (err < 0) {
            fprintf(stderr, "Failed to connect: %s\n", strerror(errno));
            close(socket_fd);
            continue;
        }

        strncpy(client_message, msg, sizeof(client_message));
        if (send(socket_fd, client_message, strlen(client_message), 0) < 0) {
            printf("Unable to send message\n");
            continue;
        }
        close(socket_fd);
        return;
    }
}

// argv[0] = Program Name
// argv[1] = OS Select
int main(int argc, char *argv[]) {
    char* msg;
    fprintf(msg, "BOOT - %s", argv[1]);
    tcp_connection(argv[1]);
}