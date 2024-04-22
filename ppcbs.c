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
#include "common.h"
#include "protconst.h"


void send_rejection_udp(struct sockaddr_in *client_address, struct data* data, int socket_fd) {
    struct rjt rejection;
    init_rjt(&rejection, be64toh(data->net_packet_number), be64toh(data->meta.session_id));
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &rejection, sizeof(struct rjt), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "Client has not received rejection of connection info.\n");
    }
}


int udp_receive_conn(int socket_fd, struct conn *connection, struct sockaddr_in *client_address) {
    unset_timeout(socket_fd);
    ssize_t received_length;
    int flags = 0;
    socklen_t address_length = (socklen_t) sizeof(&client_address);
    received_length = recvfrom(socket_fd, connection, sizeof(struct conn), flags,
                               (struct sockaddr *) client_address, &address_length);
    if (received_length < 0) {
        fprintf(stderr, "Did not receive connection.\n");
        return 0;
    }
    if (received_length != sizeof(struct conn)) {
        fprintf(stderr, "received_length != sizeof(struct conn)");
        return 0;
    }
    if (connection->meta.packet_type_id != 1) {
        if (connection->meta.packet_type_id == 4) {
            struct data *data = (struct data *) connection;
            send_rejection_udp(client_address, data, socket_fd);
        }
        fprintf(stderr, "Server expects connection packet first!\n");
        return 0;
    }
    if (connection->protocol_id == 1) {
        fprintf(stderr, "Incoming tcp connection to udp server!\n");
        return 0;
    }
    if (connection->protocol_id != 2 && connection->protocol_id != 3) {
        fprintf(stderr, "unknown connection to udp server!\n");
        return 0;
    }
    return 1;
}

ssize_t udp_send_con_acc(int socket_fd, struct sockaddr_in *client_address, uint64_t session_id) {
    struct con_acc con_acc;
    init_con_acc(&con_acc, session_id);
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &con_acc, sizeof(struct con_acc), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
   return sent;
}

int udp_receive_data_packet(int socket_fd, struct data *data, uint64_t session_id, uint64_t *currently_received) {
    ssize_t received_length;
    int flags = 0;
    struct sockaddr_in incoming_address;
    socklen_t address_length = (socklen_t) sizeof(incoming_address);
    received_length = recvfrom(socket_fd, data, sizeof(struct data) + MAX_PACKET_SIZE, flags,
                               (struct sockaddr *) &incoming_address, &address_length);
    if (received_length < 0) {
        if (errno == EAGAIN) {
            fprintf(stderr, "timeout\n");
        } else {
        fprintf(stderr, "recvfrom failed\n");
        }
        return -1;
    }
    if (data->meta.packet_type_id != 4) {
        fprintf(stderr, "server expects data packet!\n");
        return 0;
    }
    if (be64toh(data->meta.session_id) != session_id) {
        fprintf(stderr, "data with wrong session_id!\n");
        send_rejection_udp(&incoming_address, data, socket_fd);
    }
    if(received_length != sizeof(struct data) + be32toh(data->net_packet_bytes)) {
        fprintf(stderr, "received_length != sizeof(struct data) + be32toh(data->net_packet_bytes)\n");
        return -1;
    }
    fprintf(stderr, "received data packet with number %" PRIu64 "\n", be64toh(data->net_packet_number));
    fprintf(stderr, "received data packet with size %" PRIu32 "\n", be32toh(data->net_packet_bytes));
    ssize_t wrote = write(STDOUT_FILENO, data->data, be32toh(data->net_packet_bytes));
    if (wrote < 0) {
        fprintf(stderr, "write failed\n");
        return -1;
    }
    *currently_received += be32toh(data->net_packet_bytes);
    return 1;
}

void udp_send_rcvd(int socket_fd, struct sockaddr_in *client_address, uint64_t session_id) {
    struct rcvd received;
    int flags = 0;
    init_rcvd(&received, session_id);
    ssize_t sent = sendto(socket_fd, &received, sizeof(struct rcvd), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));

    if (sent != sizeof(struct rcvd)) {
        fprintf(stderr, "partial / failed write\n");
    }
    fprintf(stderr, "received all data\n");
}


static void udp_server_no_retransmit_recv(uint64_t session_id, struct sockaddr_in* client_address,
                                          uint64_t sequence_length, int socket_fd, char *buffer) {

    if (udp_send_con_acc(socket_fd, client_address, session_id) < 0) {
        fprintf(stderr, "con_acc could not be sent, closing connection...\n");
        return;
    }
    uint64_t currently_received = 0;
    struct sockaddr_in  incoming_address;
    uint64_t expected_packet_number = 0;
    while (currently_received < sequence_length) {
        int received = udp_receive_data_packet(socket_fd, (struct data *) buffer, session_id, &currently_received);
        if (received < 0) {
            fprintf(stderr, "data could not be received, closing connection...\n");
            return;
        }
    }
    udp_send_rcvd(socket_fd, client_address, session_id);
}


static void udp_server_retransmit_recv(uint64_t session_id, struct sockaddr_in client_address,
                                       uint64_t sequence_length, int socket_fd, char *buffer) {

}


static void udp_server_run(uint16_t port) {
    char receive_buffer[sizeof (struct data) + MAX_PACKET_SIZE];
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket, cannot run server\n");
    }
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET; // IPv4
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // Listening on all interfaces.
    server_address.sin_port = htons(port);
    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
    while (true) {
        struct conn connection;
        struct sockaddr_in client_address;
        if (!udp_receive_conn(socket_fd, &connection, &client_address)) {
            continue;
        }
        set_timeout(socket_fd);

        uint64_t session_id = be64toh(connection.meta.session_id);
        uint64_t sequence_length = be64toh(connection.net_sequence_length);
        uint8_t protocol_id = connection.protocol_id;
        if (protocol_id == 2) {
            udp_server_no_retransmit_recv(session_id, &client_address, sequence_length, socket_fd, receive_buffer);
        } else if (protocol_id == 3) {
            udp_server_retransmit_recv(session_id, client_address, sequence_length, socket_fd, receive_buffer);
        }
        else {
            fprintf(stderr, "unknown protocol!\n");
        }
    }
}


static void tcp_server_run(uint16_t port) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //TODO
}

void run_server(uint8_t protocol_type, uint16_t port) {
    if (protocol_type == 1) {
        tcp_server_run(port);
    }
    if (protocol_type == 2) {
        udp_server_run(port);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", argv[0]);
    }
    run_server(read_protocol(argv[1]), read_port(argv[2]));
    return 0;
}