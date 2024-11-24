#include <arpa/inet.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
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

int find_slot_by_id(int fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if(client_states[i].fd == fd) {
            return i;
        }
    }
    return -1;
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
    int listen_fd, client_fd;
    struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
    socklen_t client_addr_size = sizeof(client_addr);

    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1;
    int opt = 1;

    fd_set read_fds, write_fds;
    int connection_fd, free_slot;

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Initialise client states
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_states[i].fd = -1; // = free slot
        client_states[i].state = STATE_NEW;
        memset(&client_states[i].buffer, '\0', BUFFER_SIZE);
    }

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    if (bind(listen_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    if (listen(listen_fd, 10) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    memset(fds, 0, sizeof(fds));
    fds[0].fd = listen_fd;
    fds[0].events = POLLIN;
    nfds = 1;

    while (1) {
        int ii = 1;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_states[i].fd != -1) {
                fds[ii].fd = client_states[i].fd;
                fds[ii].events = POLLIN;
                ii++;
            }
        }

        // Wait for an event
        int n_events = poll(fds, nfds, -1);
        if (n_events == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        }

        // Check for new connection
        if (fds[0].revents & POLLIN) {
            if ((connection_fd = accept(listen_fd, (struct sockaddr *) &client_addr, &client_addr_size)) == -1) {
                perror("accept");
                continue;
            }

            free_slot = find_free_slot();

            if (free_slot == -1) {
                printf("No free slot found: closing connection\n");
                close(connection_fd);
            } else {
                client_states[free_slot].fd = connection_fd;
                client_states[free_slot].state = STATE_CONNECTED;
                nfds++;
                printf("New connection made from %s - port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
            n_events--;
        }

        for (int i = i; i <= nfds && n_events > 0; i++) {
            if (fds[i].revents & POLLIN) {
                n_events--;
                int fd = fds[i].fd;
                int slot = find_slot_by_id(fd);
                ssize_t bytes_read = read(fd, &client_states[slot].buffer, sizeof(client_states[slot].buffer));
                if (bytes_read <= 0) {
                    close(fd);
                    if (slot == -1) {

                    } else {
                        client_states[slot].fd = -1;
                        client_states[slot].state = STATE_DISCONNECTED;
                        nfds--;
                        printf("Client disconnected\n");
                    }
                } else {
                    printf("Received data from client: %s\n", client_states[slot].buffer);
                }
            }
        }
    }

    close(listen_fd);
    return EXIT_SUCCESS;
}
