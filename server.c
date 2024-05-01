#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>

#include "protocol.h"

void write_to_client(int client_fd) {
	char buffer[4096] = {0};
	proto_hdr_t *header = (proto_hdr_t*)buffer;

	header->type = htonl(PROTO_HELLO);
    header->length = sizeof(int);
	int real_length = header->length;
	header->length = htons(header->length);

    int* data = (int*)&header[1];
	*data = htonl(1);

	write(client_fd, header, sizeof(proto_hdr_t) + real_length);
    printf("Write\n");
}

int main(void)
{
    struct sockaddr_in server_info = { 0 };
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = 0;
    server_info.sin_port = htons(5555);
    struct sockaddr_in client_info = { 0 };
    int client_size = 0;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        return 0;
    }
    if (bind(server_fd, (struct sockaddr*)&server_info, sizeof(server_info)) == -1) {
        perror("bind");
        return 0;
    }
    if (listen(server_fd, 0) == -1) {
        perror("listen");
        return 0;
    }

    while (true) {
        socklen_t client_addr_len = sizeof(client_info);
        printf("Ready to accept\n");

        int client_fd = accept(server_fd, (struct sockaddr*)&client_info, &client_size);
        // Wait here

        if (client_fd == -1) {
            perror("accept");
            return 0;
        }

        printf("Connection made\n");
        write_to_client(client_fd);
        close(client_fd);
    }

    close(server_fd);
    return 0;
}
