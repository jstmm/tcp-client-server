#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv)
{
    if (argc != 2) {
        printf("Usage: %s <ip address>\n", argv[0]);
        return 1;
    }

    struct sockaddr_in server_info = { 0 };
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = inet_addr(argv[1]);
    server_info.sin_port = htons(5555);
    int client_size = 0;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        return 0;
    }
    if (connect(server_fd, (struct sockaddr*)&server_info, sizeof(server_info))) {
        perror("connect");
        return 0;
    }
    printf("Connection made\n");
    sleep(2);

    close(server_fd);
    return 0;
}
