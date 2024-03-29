//
// Created by Jan Jagodziński on 29/03/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>
#include "err.h"
#include "packets.h"

#define BUFFER_SIZE 30000
uint16_t read_protocol(char *input) {
    if (strcmp(input, "tcp") == 0) {
        return 1;
    } else if (strcmp(input, "udp") == 0) {
        return 2;
    } else {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", "ppcbs");
    }
}

static uint16_t read_port(char const *string) {
    char *endptr;
    unsigned long port = strtoul(string, &endptr, 10);
    if ((port == ULONG_MAX && errno == ERANGE) || *endptr != 0 || port == 0 || port > UINT16_MAX) {
        fatal("%s is not a valid port number", string);
    }
    return (uint16_t) port;
}

static void udp_server(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
    //to delete
    printf("listening on port %" PRIu16 "\n", port);
    char buffer[BUFFER_SIZE];
    ssize_t received_length;

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int flags = 0;
        struct sockaddr_in client_address;
        socklen_t address_length = (socklen_t) sizeof(client_address);
        received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, flags,
                                   (struct sockaddr *) &client_address, &address_length);
        if (received_length < 0) {
            syserr("recvfrom");
        }
        if (received_length != sizeof(struct conn)) {
            fatal("received_length != sizeof(struct conn)");
        }
        struct conn * connection;
        memcpy(connection, buffer, sizeof(struct conn));
        if (connection->meta.type_id != 1) {
            fprintf(stderr, "udp server is not connected!\n");
            continue;
        }
        if (connection->protocol_id == 1) {
            fprintf(stderr, "incoming tcp connection to udp server!\n");
            continue;
        }




    }

    ssize_t received_length;
    do {
        static char buffer[10];
        memset(buffer, 0, sizeof(buffer));

        int flags = 0;
        struct sockaddr_in client_address;
        socklen_t address_length = (socklen_t) sizeof(client_address);

        received_length = recvfrom(socket_fd, buffer, 10, flags,
                                   (struct sockaddr *) &client_address, &address_length);
        if (received_length < 0) {
            syserr("error on datagram from client socket");
        }

        printf("received message: %s\n", buffer);
    } while (received_length >= 0);

    if (close(socket_fd) < 0) {
        syserr("close");
    }
}

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", argv[0]);
    }
    int protocol_type = read_protocol(argv[1]); // 1- tcp, 2 - udp
    uint16_t port = read_port(argv[2]);
    if (protocol_type == 1) {

    }
    else {

    }


}