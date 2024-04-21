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
#include "packets.h"

char* read_data(size_t* full_size) {
    size_t buffer_size = 1024;
    char* buffer = malloc(buffer_size);
    size_t free_space = 1024;
    size_t currently_read = 0;
    while (1) {
        ssize_t bytes_read = read(STDIN_FILENO, buffer + currently_read, free_space);
        if (bytes_read < 0) {
            syserr("read");
        }
        if (bytes_read == 0) {
            break;
        }
        currently_read += bytes_read;
        free_space -= bytes_read;
        if (free_space == 0) {
            char * temp = buffer;
            buffer = realloc(buffer, 2 * buffer_size);
            buffer_size = 2 * buffer_size;
            if (buffer == NULL) {
                free(temp);
                fatal("realloc");
            }
            free_space = buffer_size - currently_read;
        }
    }
    *full_size = currently_read;
    return buffer;
}



uint64_t generate_session_id() {
    uint64_t result = 0;
    for (int i = 0; i < 8; i++) {
        result = (result << 8) | (rand() & 0xFF);
    }
    return result;
}



static uint16_t read_port(char const *string) {
    char *endptr;
    errno = 0;
    unsigned long port = strtoul(string, &endptr, 10);
    if (errno != 0 || *endptr != 0 || port == 0 || port > UINT16_MAX) {
        fatal("%s is not a valid port number", string);
    }
    return (uint16_t) port;
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
void run_client_udp(char const * host, uint16_t port, char* buffer, size_t full_size) {
    struct sockaddr_in server_address = get_server_address(host, port);
    char const *server_ip = inet_ntoa(server_address.sin_addr);
    uint16_t server_port = ntohs(server_address.sin_port);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    struct conn connection;
    uint64_t session_id = generate_session_id();
    //printf("session_id: %" PRIu64 "\n", session_id);
    init_conn(&connection, 2, full_size, session_id);
    ssize_t sent_length = sendto(socket_fd, &connection, sizeof(connection), 0,
                                 (struct sockaddr *) &server_address, sizeof(server_address));
    if (sent_length < 0) {
        syserr("sendto");
    }
    if (sent_length != sizeof(connection)) {
        fatal("partial / failed write");
    }
    struct con_acc con_acc;

    ssize_t received_length = recvfrom(socket_fd, &con_acc, sizeof(con_acc), 0, NULL, NULL);
    if(received_length < 0) {
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
    if (con_acc.meta.session_id != connection.meta.session_id) {
        fatal("session_id mismatch");
    }
    //printf("received con_acc packet\n");
    size_t sent_by_now = 0;
    uint64_t sequence_number = 0;
    while (sent_by_now < full_size) {
        struct data data_pckt;
        size_t to_send_in_this_packet = full_size - sent_by_now >= MAX_PACKET_SIZE ? MAX_PACKET_SIZE : full_size - sent_by_now;
        init_data(&data_pckt, sequence_number, to_send_in_this_packet, buffer + sent_by_now, session_id);
        sent_length = sendto(socket_fd, &data_pckt, sizeof(struct data), 0,
                             (struct sockaddr *) &server_address, sizeof(server_address));
        if (sent_length < 0) {
            syserr("sendto");
        }
        if (sent_length != sizeof(struct data)) {
            fatal("partial / failed write");
        }
        sequence_number++;
        sent_by_now += to_send_in_this_packet;
        fprintf(stderr, "sent %ld bytes\n", sent_by_now);
        usleep(500);
    }
    struct rjt rejection;
    ssize_t received = recvfrom(socket_fd, &rejection, sizeof(rejection), 0, NULL, NULL);
    if (received < 0) {
        syserr("recvfrom");
    }
    if (rejection.meta.packet_type_id == 6) {
        if (be64toh(rejection.meta.session_id) != session_id) {
            fatal("session_id mismatch");
        }
        fprintf(stderr, "received rjt packet\n");
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
void run_client_udpr(char const* host, uint16_t port, char* buffer, size_t full_size) {

}

void run_client_tcp(char const* host, uint16_t port, char* buffer, size_t full_size) {

}


void run_client(uint8_t protocol_type, char const* host, uint16_t port) {
    size_t full_size;
   char* buffer = read_data(&full_size);
    if (protocol_type == 1) {
        run_client_tcp(host, port, buffer, full_size);
    } else if (protocol_type == 2) {
        run_client_udp(host, port, buffer, full_size);
    } else if (protocol_type == 3) {
        run_client_udpr(host, port, buffer, full_size);
    } else {
        fatal("unknown protocol type");
    }
}

int main (int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <host> <port>  ...\n", argv[0]);
    }

    run_client(read_protocol(argv[1]), argv[2], read_port(argv[3]));


    //TODO read input

    // Create a socket.

}