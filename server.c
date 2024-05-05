#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>

#include "protocol.h"

void write_to_client(int client_fd) {
	char buffer[4096] = {0};
	proto_hdr_t *header = (proto_hdr_t *) buffer;
    int data_length = sizeof(int);

	header->type = htonl(PROTO_HELLO);
	header->length = htons(data_length);

    int *data = (int *) &header[1];
	*data = htonl(1);

	write(client_fd, header, sizeof(proto_hdr_t) + data_length);
    printf("Write\n");
}

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
    socklen_t client_addr_size;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = 0;
    server_addr.sin_port = htons(5555);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 0) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    while (true) {
        client_addr_size = sizeof(client_addr);
        printf("Ready to accept\n");
        client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);
        // Waiting for client connect()

        if (client_fd == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        printf("Connection made\n");
        write_to_client(client_fd);

        close(client_fd);
        printf("Connection closed\n");
        printf("-------------------------------------\n");
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
