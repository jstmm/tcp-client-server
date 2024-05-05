#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#include "protocol.h"

void read_from_server(int server_fd) {
	char buffer[4096] = {0};
	proto_hdr_t *header = (proto_hdr_t *) buffer;
	
    read(server_fd, header, sizeof(proto_hdr_t) + sizeof(int));
    // Waiting for server write()

	header->type = ntohl(header->type);
	header->length = ntohs(header->length);

	int *data = (int *) &header[1];
	*data = ntohl(*data);

    printf("Message received\n");

    if (header->type != PROTO_HELLO) {
        printf("Protocol mismatch, failing\n");
        return;
    }

	if (*data != 1) {
		printf("Protocol version mismatch, failing\n");
		return;
	}
}

int main(int argc, char** argv)
{
    int server_fd;
    struct sockaddr_in server_addr = { 0 };

    if (argc != 2) {
        printf("Usage: %s <ip address>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);
    server_addr.sin_port = htons(5555);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connection made\n");
    read_from_server(server_fd);

    close(server_fd);
    printf("Connection closed\n");
    return EXIT_SUCCESS;
}
