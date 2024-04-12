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

#define BUFFER_SIZE 64000


static uint16_t read_protocol(char *input) {
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

static void tcp_server(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);


}

int receive_conn(char *buffer, int socket_fd, struct conn *connection, struct sockaddr_in *client_address) {
    ssize_t received_length;
    int flags = 0;
    socklen_t address_length = (socklen_t) sizeof(&client_address);
    received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, flags,
                               (struct sockaddr *) client_address, &address_length);
    if (received_length < 0) {
        syserr("recvfrom");
    }
    if (received_length != sizeof(struct conn)) {
        fprintf(stderr, "received_length != sizeof(struct conn)");
        return 0;
    }
    memcpy(connection, buffer, sizeof(struct conn));
    if (connection->meta.packet_type_id != 1) {
        fprintf(stderr, "server expects connection packet first!\n");
        return 0;
    }
    if (connection->protocol_id == 1) {
        fprintf(stderr, "incoming tcp connection to udp server!\n");
        return 0;
    }
    if (connection->protocol_id != 2 && connection->protocol_id != 3) {
        fprintf(stderr, "unknown connection to udp server!\n");
        return 0;
    }
    return 1;
}


static void udp_server_no_retransmit_recv(uint64_t session_id, struct sockaddr_in* client_address,
                                          uint64_t sequence_length, int socket_fd, char *buffer) {
    struct con_acc con_acc;
    con_acc.meta.packet_type_id = 2;
    con_acc.meta.session_id = htobe64(session_id);
    char const *client_ip = inet_ntoa((*client_address).sin_addr);
    uint16_t client_port = ntohs((*client_address).sin_port);
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &con_acc, sizeof(struct con_acc), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "sendto failed\n");
        return;
    }
    uint64_t currently_received = 0;
    struct sockaddr_in  incoming_address;
    while (currently_received < sequence_length) {
        ssize_t received_length;
        socklen_t address_length = (socklen_t) sizeof(incoming_address);
        received_length = recvfrom(socket_fd, buffer, BUFFER_SIZE, flags,
                                   (struct sockaddr *) &incoming_address, &address_length);
        if (received_length < 0) {
            fprintf(stderr, "recvfrom failed\n");
            return;
        }
        if()
        struct data *data = (struct data *) buffer;
        if (data->meta.packet_type_id != 4) {
            fprintf(stderr, "server expects data packet!\n");
            return;
        }
        if (data->meta.session_id != session_id) {
            fprintf(stderr, "server expects data packet with correct session_id!\n");
            return;
        }
        if (data->net_packet_number != currently_received) {
            fprintf(stderr, "server expects data packet with correct net_packet_number!\n");
            return;
        }
        currently_received += data->net_packet_bytes;
        struct acc acc;
        acc.meta.packet_type_id = 5;
        acc.meta.session_id = htobe64(session_id);
        acc.net_packet_number = htobe64(data->net_packet_number);
        sent = sendto(socket_fd, &acc, sizeof(struct acc), flags,
                      (struct sockaddr *) &client_address, (socklen_t) sizeof(client_address));
        if (sent < 0) {
            fprintf(stderr, "sendto failed\n");
            return;
        }
    }
}


static void udp_server_retransmit_recv(uint64_t session_id, struct sockaddr_in client_address,
                                       uint64_t sequence_length, int socket_fd, char *buffer) {

}


static void udp_server_run(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
    //TODO: delete
    printf("listening on port %" PRIu16 "\n", port);
    char buffer[BUFFER_SIZE];


    while (true) {

        struct conn connection;
        struct sockaddr_in client_address;
        uint64_t sequence_length = be64toh(connection.net_sequence_length);
        if (!receive_conn(buffer, socket_fd, &connection, &client_address)) {
            continue;
        }
        uint64_t session_id = be64toh(connection.meta.session_id);
        uint8_t protocol_id = connection.protocol_id;

        if (protocol_id == 2) {
            udp_server_no_retransmit_recv(session_id, &client_address, sequence_length, socket_fd, buffer);
        } else if (protocol_id == 3) {
            udp_server_retransmit_recv(session_id, client_address, sequence_length, socket_fd, buffer);
        }
    }


    if (close(socket_fd) < 0) {
        syserr("close");
    }

}

void run_server(int protocol_type, uint16_t port) {
    if (protocol_type == 1) {
        tcp_server(port);
    }
    if (protocol_type == 2) {
        udp_server_run(port);
    }
    // read_protocol ensures that protocol_type is either 1 or 2
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", argv[0]);
    }
    int protocol_type = read_protocol(argv[1]); // 1- tcp, 2 - udp
    uint16_t port = read_port(argv[2]);
    run_server(protocol_type, port);
}