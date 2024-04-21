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


void send_rejection(struct sockaddr_in *client_address, struct data* data, int socket_fd) {
    struct rjt rejection;
    init_rjt(&rejection, be64toh(data->net_packet_number), be64toh(data->meta.session_id));
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &rejection, sizeof(struct rjt), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "client has not received rejection of connection info. damn.\n");
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

static void tcp_server_run(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);


}

int udp_receive_conn(char *buffer, int socket_fd, struct conn *connection, struct sockaddr_in *client_address) {
    ssize_t received_length;
    int flags = 0;
    socklen_t address_length = (socklen_t) sizeof(&client_address);
    received_length = recvfrom(socket_fd, buffer, sizeof(struct conn), flags,
                               (struct sockaddr *) client_address, &address_length);
    if (received_length < 0) {
        fprintf(stderr, "did not receive connection!\n");
        return 0;
    }
    if (received_length != sizeof(struct conn)) {
        fprintf(stderr, "received_length != sizeof(struct conn)");
        return 0;
    }
    memcpy(connection, buffer, sizeof(struct conn));
    if (connection->meta.packet_type_id != 1) {
        if (connection->meta.packet_type_id == 4) {
            struct data *data = (struct data *) connection;
            send_rejection(client_address, data, socket_fd);
        }
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

    fprintf(stderr, "sequence length %" PRIu64 "\n", sequence_length);
    struct con_acc con_acc;
    init_con_acc(&con_acc, session_id);
    //char const *client_ip = inet_ntoa((*client_address).sin_addr);
    //uint16_t client_port = ntohs((*client_address).sin_port);
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &con_acc, sizeof(struct con_acc), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "sendto failed\n");
        return;
    }

    uint64_t currently_received = 0;
    struct sockaddr_in  incoming_address;
    uint64_t expected_packet_number = 0;
    while (currently_received < sequence_length) {
        ssize_t received_length;
        socklen_t address_length = (socklen_t) sizeof(incoming_address);
        received_length = recvfrom(socket_fd, buffer, sizeof(struct data), flags,
                                   (struct sockaddr *) &incoming_address, &address_length);
        if (received_length < 0) {
            fprintf(stderr, "recvfrom failed\n");
            close(socket_fd);
            return;
        }
        struct data *data = (struct data *) buffer;

        if (data->meta.packet_type_id != 4) {
            fprintf(stderr, "server expects data packet!\n");
            continue;
        }
        if (be64toh(data->meta.session_id) != session_id) {
            fprintf(stderr, "data with wrong session_id!\n");
            struct rjt rejection;
            init_rjt(&rejection, data->net_packet_number, session_id);
            sent = sendto(socket_fd, &rejection, sizeof(struct rjt), flags,
                          (struct sockaddr *) &incoming_address, (socklen_t) sizeof(*client_address));
            if (sent < 0) {
                fprintf(stderr, "client has not received rejection of connection info. damn.\n");
            }
        }
        fprintf(stderr, "received data packet with number %" PRIu64 "\n", be64toh(data->net_packet_number));
        if (be64toh(data->net_packet_number) != expected_packet_number || be32toh(data->net_packet_bytes) > MAX_PACKET_SIZE) {
            fprintf(stderr, "wrong data - sad to say, we say goodbye!\n");
            struct rjt rejection;
            init_rjt(&rejection, data->net_packet_number, session_id);
            sent = sendto(socket_fd, &rejection, sizeof(struct rjt), flags,
                          (struct sockaddr *) &incoming_address, (socklen_t) sizeof(*client_address));
            if (sent < 0) {
                fprintf(stderr, "client has not received rejection of connection info. damn.\n");
            }
            return;
        }
        fprintf(stderr, "received data packet with size %" PRIu32 "\n", be32toh(data->net_packet_bytes));
            currently_received += be32toh(data->net_packet_bytes);
            expected_packet_number++;
            ssize_t wrote = write(STDOUT_FILENO, data->data, be32toh(data->net_packet_bytes));
            if (wrote < 0) {
                fprintf(stderr, "write failed\n");
                return;
            }
    }
    struct rcvd received;
    init_rcvd(&received, session_id);
    sent = sendto(socket_fd, &received, sizeof(struct rcvd), flags,
                  (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "sendto failed\n");
        return;
    }
    if (sent != sizeof(struct rcvd)) {
        fprintf(stderr, "partial / failed write\n");
        return;
    }
    fprintf(stderr, "received all data\n");
}


static void udp_server_retransmit_recv(uint64_t session_id, struct sockaddr_in client_address,
                                       uint64_t sequence_length, int socket_fd, char *buffer) {

}


static void udp_server_run(uint16_t port) {
//TODO we have to use one socket!!!!!!
    char buffer[sizeof (struct data)];
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

    while (true) {
        //TODO: delete
        fprintf(stderr, "listening on port %" PRIu16 "\n", port);

        struct conn connection;
        struct sockaddr_in client_address;
        if (!udp_receive_conn(buffer, socket_fd, &connection, &client_address)) {
            continue;
        }
        uint64_t session_id = be64toh(connection.meta.session_id);
        uint64_t sequence_length = be64toh(connection.net_sequence_length);
        fprintf(stderr, "sequence length: %" PRIu64 "\n", sequence_length);
        uint8_t protocol_id = connection.protocol_id;
        printf("received connection packet with session id %" PRIu64 "\n", session_id);
        if (protocol_id == 2) {
            udp_server_no_retransmit_recv(session_id, &client_address, sequence_length, socket_fd, buffer);
        } else if (protocol_id == 3) {
            udp_server_retransmit_recv(session_id, client_address, sequence_length, socket_fd, buffer);
        }
        else {
            fprintf(stderr, "unknown protocol!\n");
        }
    }
    if (close(socket_fd) < 0) {
        syserr("close");
    }
}

void run_server(uint8_t protocol_type, uint16_t port) {
    if (protocol_type == 1) {
        tcp_server_run(port);
    }
    if (protocol_type == 2) {
        udp_server_run(port);
    }
    // read_protocol ensures that protocol_type is either 1 or 2
    return;
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", argv[0]);
    }
    run_server(read_protocol(argv[1]), read_port(argv[2]));
    return 0;
}