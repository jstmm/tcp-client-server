#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>

#include "protocol.h"

#define PORT 5555
#define MAX_CLIENTS 1024
#define BUFFER_SIZE 4096

typedef enum {
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED
} state_e;

typedef struct {
    int fd;
    state_e state;
    char buffer[BUFFER_SIZE];
} clientstate_t;

clientstate_t client_states[MAX_CLIENTS];

int find_free_slot() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_states[i].fd == -1)
            return i;
    }
    return -1; // No free slot found
}

void read_from_client(int i) {
    ssize_t bytes_read = read(client_states[i].fd,
                              &client_states[i].buffer,
                              sizeof(client_states[i].buffer));
    if (bytes_read <= 0) {
        close(client_states[i].fd);
        client_states[i].fd = -1;
        client_states[i].state = STATE_DISCONNECTED;
        printf("Client disconnected or error\n");
    } else {
        printf(">>> Data received from client (%d): %s", client_states[i].fd, client_states[i].buffer);
    }
    memset(&client_states[i].buffer, '\0', BUFFER_SIZE);
}

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
    socklen_t client_addr_size;

    fd_set read_fds, write_fds;
    int connection_fd, nfds, free_slot;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_states[i].fd = -1; // = free slot
        client_states[i].state = STATE_NEW;
        memset(&client_states[i].buffer, '\0', BUFFER_SIZE);
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        client_addr_size = sizeof(client_addr);

        // Clear FD sets
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);

        // Add listening socket to the read set
        FD_SET(server_fd, &read_fds);
        nfds = server_fd + 1;

        // Add active connections to the read set
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int current_fd = client_states[i].fd;
            if (current_fd != -1) {
                FD_SET(current_fd, &read_fds);
                if (current_fd >= nfds) nfds = current_fd + 1;
            }
        }

        printf("Wait for an input\n");
        // Wait for an input on either socket
        if (select(nfds, &read_fds, &write_fds, NULL, NULL) == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        system("clear");
        // Check for new connection
        if (FD_ISSET(server_fd, &read_fds)) {
            client_fd = accept(server_fd, (struct sockaddr *) &client_addr, &client_addr_size);
            // Waiting for client connect()

            if (client_fd == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("New connection made from %s - port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            free_slot = find_free_slot();

            if (free_slot == -1) {
                printf("No free slot found: closing connection\n");
                close(client_fd);
            } else {
                client_states[free_slot].fd = client_fd;
                client_states[free_slot].state = STATE_CONNECTED;
                nfds = client_fd + 1;
            }
        }

        for (int i = 0; i <= nfds; i++) {
            printf("Check FD %d\n", i);
            if (client_states[i].fd != -1 && FD_ISSET(client_states[i].fd, &read_fds)) {
                read_from_client(i);
            }
        }

        printf("Connection closed\n");
        printf("-------------------------------------\n");
    }

    close(server_fd);
    return EXIT_SUCCESS;
}
