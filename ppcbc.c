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
#include <limits.h>
#include <time.h>
#include "err.h"
#include "common.h"

char* read_data(size_t* full_size) {
    size_t data_to_send_size = 1024;
    char* data_to_send = malloc(data_to_send_size);
    size_t free_space = 1024;
    size_t currently_read = 0;
    while (1) {
        ssize_t bytes_read = read(STDIN_FILENO, data_to_send + currently_read, free_space);
        if (bytes_read < 0) {
            syserr("read");
        }
        if (bytes_read == 0) {
            break;
        }
        currently_read += bytes_read;
        free_space -= bytes_read;
        if (free_space == 0) {
            char * temp = data_to_send;
            data_to_send = realloc(data_to_send, 2 * data_to_send_size);
            data_to_send_size = 2 * data_to_send_size;
            if (data_to_send == NULL) {
                free(temp);
                fatal("realloc");
            }
            free_space = data_to_send_size - currently_read;
        }
    }
    *full_size = currently_read;
    return data_to_send;
}

uint64_t generate_session_id() {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 8) | (rand() & 0xFF);
    }
    return result;
}

void udp_send_conn(uint64_t full_size, int socket_fd, struct sockaddr_in server_address, uint64_t session_id) {
    struct conn connection;
    init_conn(&connection, 2, full_size, session_id);
    ssize_t sent_length = sendto(socket_fd, &connection, sizeof(connection), 0,
                                 (struct sockaddr *) &server_address, sizeof(server_address));
    if (sent_length < 0) {
        syserr("sendto");
    }
    if (sent_length != sizeof(connection)) {
        fatal("partial / failed write");
    }
    fprintf(stderr, "sent conn packet\n");
}

void udp_recv_con_acc(int socket_fd, uint64_t session_id) {
   fprintf(stderr, "waiting for con_acc packet\n");
    struct con_acc con_acc;
    ssize_t received_length = recvfrom(socket_fd, &con_acc, sizeof(con_acc), 0, NULL, NULL);
    if(received_length < 0) {
        if (errno == EAGAIN) {
            fatal("timeout!");
        }
        syserr("recvfrom");
    }
    if(received_length != sizeof(con_acc)) {
        fatal("partial / failed read");
    }
    if (con_acc.meta.packet_type_id != 2) {
        if (con_acc.meta.packet_type_id == 3) {
            fatal("connection got rejected!");
        }
        fatal("expected con_acc packet or con_rjt packet");
    }
    if (be64toh(con_acc.meta.session_id) != session_id) {
        fatal("session_id mismatch");
    }
    fprintf(stderr, "received con_acc packet\n");
}



void udp_send_data_packet(size_t* sent_by_now, size_t full_size, int socket_fd, struct sockaddr_in *server_address, uint64_t session_id,
             uint64_t *sequence_number, char* data_to_send) {
    size_t to_send_in_this_packet = full_size - *sent_by_now >= MAX_PACKET_SIZE ? MAX_PACKET_SIZE : full_size - *sent_by_now;
    struct data * data_packet = malloc(sizeof(struct data) + to_send_in_this_packet);
    init_data(data_packet, *sequence_number, to_send_in_this_packet, data_to_send + *sent_by_now, session_id);
    ssize_t sent_length = sendto(socket_fd, data_packet, sizeof(struct data) + to_send_in_this_packet, 0,
                                 (struct sockaddr *) server_address, sizeof(*server_address));
    free(data_packet);
    if (sent_length < 0) {
        syserr("sendto");
    }
    if ((uint64_t)sent_length != to_send_in_this_packet + (uint64_t)sizeof(struct data)) {
        fatal("partial / failed write");
    }
    *sent_by_now += to_send_in_this_packet;
    *sequence_number += 1;
}

void udp_receive_rjt_or_rcvd(int socket_fd, uint64_t session_id) {
    struct rjt rejection;
    ssize_t received = recvfrom(socket_fd, &rejection, sizeof(rejection), 0, NULL, NULL);
    if (received < 0) {
        if (errno == EAGAIN) {
            fatal("timeout!");
        }
        syserr("recvfrom");
    }
    if (rejection.meta.packet_type_id == 6) {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        fatal("received rjt packet");
    }
    else if (rejection.meta.packet_type_id == 7) {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        fprintf(stderr, "received rcvd packet\n");
    }
    else {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        fatal("expected rjt or rcvd packet");
    }
}



static struct sockaddr_in get_server_address(char const *host, uint16_t port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    struct addrinfo *address_result;
    int errcode = getaddrinfo(host, NULL, &hints, &address_result);
    if (errcode != 0) {
        fatal("getaddrinfo: %s", gai_strerror(errcode));
    }

    struct sockaddr_in send_address;
    send_address.sin_family = AF_INET;   // IPv4
    send_address.sin_addr.s_addr =       // IP address
            ((struct sockaddr_in *) (address_result->ai_addr))->sin_addr.s_addr;
    send_address.sin_port = htons(port); // port from the command line

    freeaddrinfo(address_result);

    return send_address;
}


void run_client_udp(char const * host, uint16_t port, char* data_to_send, size_t full_size) {
    struct sockaddr_in server_address = get_server_address(host, port);
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    set_timeout(socket_fd);
    uint64_t session_id = generate_session_id();
    udp_send_conn(full_size, socket_fd, server_address, session_id);
    udp_recv_con_acc(socket_fd, session_id);
    size_t sent_by_now = 0;
    uint64_t sequence_number = 0;

    while (sent_by_now < full_size) {
        udp_send_data_packet(&sent_by_now, full_size, socket_fd, &server_address, session_id, &sequence_number, data_to_send);
        usleep(500);
    }
    udp_receive_rjt_or_rcvd(socket_fd, session_id);
}
void run_client_udpr(char const* host, uint16_t port, char* data_to_send, size_t full_size) {
}

void tcp_send_conn(int socket_fd, uint64_t full_size, uint64_t session_id) {
    struct conn connection;
    init_conn(&connection, 1, full_size, session_id);
    ssize_t sent_length = writen(socket_fd, &connection, sizeof(connection));
    if (sent_length < 0) {
        syserr("write");
    }
    if (sent_length != sizeof(connection)) {
        fatal("partial / failed write");
    }
}


void tcp_receive_con_acc(int socket_fd, uint64_t session_id) {
    struct con_acc con_acc;
    ssize_t received_length = readn(socket_fd, &con_acc, sizeof(con_acc));
    if (received_length < 0) {
        if (errno == EAGAIN) {
            fatal("timeout!");
        }
        syserr("read");
    }
    if (received_length != sizeof(con_acc)) {
        fatal("partial / failed read");
    }
    if (con_acc.meta.packet_type_id != 2) {
        if (con_acc.meta.packet_type_id == 3) {
            fatal("connection got rejected!");
        }
        fatal("expected con_acc packet or con_rjt packet");
    }
    if (be64toh(con_acc.meta.session_id) != session_id) {
        fatal("session_id mismatch");
    }

}

void tcp_send_data_packet(int socket_fd, size_t* sent_by_now, size_t full_size, uint64_t session_id, uint64_t* sequence_number, char* data_to_send) {
    size_t to_send_in_this_packet = full_size - *sent_by_now >= MAX_PACKET_SIZE ? MAX_PACKET_SIZE : full_size - *sent_by_now;
    struct data * data_packet = malloc(sizeof(struct data) + to_send_in_this_packet);
    init_data(data_packet, *sequence_number, to_send_in_this_packet, data_to_send + *sent_by_now, session_id);
    ssize_t sent_length = writen(socket_fd, data_packet, sizeof(struct data) + to_send_in_this_packet);
    free(data_packet);
    if (sent_length < 0) {
        syserr("write");
    }
    fprintf(stderr, "%lu\n", *sequence_number);
    if ((uint64_t)sent_length != to_send_in_this_packet + (uint64_t)sizeof(struct data)) {
        fatal("partial / failed write");
    }
    *sent_by_now += to_send_in_this_packet;
    *sequence_number += 1;
}

void tcp_receive_rjt_or_rcvd (int socket_fd, uint64_t session_id) {
    struct rjt rejection;
    ssize_t received = readn(socket_fd, &rejection, sizeof(struct rcvd));
    if (received < 0) {
        if (errno == EAGAIN) {
            fatal("timeout!");
        }
        syserr("read");
    }
    if (rejection.meta.packet_type_id == 6) {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        received = readn(socket_fd, &rejection, sizeof(struct rjt) - sizeof(struct rcvd));
        fatal("received rjt packet with packet_number %" PRIu64, be64toh(rejection.net_packet_number));
    }
    else if (rejection.meta.packet_type_id == 7) {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        fprintf(stderr, "received rcvd packet\n");
    }
    else {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        fatal("expected rjt or rcvd packet");
    }
}


void run_client_tcp(char const* host, uint16_t port, char* data_to_send, size_t full_size) {
    struct sockaddr_in server_address = get_server_address(host, port);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    set_timeout(socket_fd);
    if (connect(socket_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
        syserr("connect");
    }
    uint64_t session_id = generate_session_id();
    tcp_send_conn(socket_fd, full_size, session_id);
    fprintf(stderr, "sent conn packet\n");
    tcp_receive_con_acc(socket_fd, session_id);
    fprintf(stderr, "received con_acc packet\n");
    size_t sent_by_now = 0;
    uint64_t sequence_number = 0;
    while (sent_by_now < full_size) {
        tcp_send_data_packet(socket_fd, &sent_by_now, full_size, session_id, &sequence_number, data_to_send);
    }
    fprintf(stderr, "sent all data\n");
    tcp_receive_rjt_or_rcvd(socket_fd, session_id);
}


void run_client(uint8_t protocol_type, char const* host, uint16_t port) {
    size_t full_size;
    char* data_to_send = read_data(&full_size);
    if (protocol_type == 1) {
        run_client_tcp(host, port, data_to_send, full_size);
    } else if (protocol_type == 2) {
        run_client_udp(host, port, data_to_send, full_size);
    } else if (protocol_type == 3) {
        run_client_udpr(host, port, data_to_send, full_size);
    } else {
        free(data_to_send);
        fatal("unknown protocol type");
    }
    free(data_to_send);
}

int main (int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <host> <port>  ...\n", argv[0]);
    }
    run_client(read_protocol(argv[1]), argv[2], read_port(argv[3]));
    return 0;

}