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


uint64_t generate_session_id() {
    uint64_t result = 0;
    for(int i = 0; i < 8; i++) {
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
void run_client_udp(char const * host, uint16_t port) {
    struct sockaddr_in server_address = get_server_address(host, port);
    char const *server_ip = inet_ntoa(server_address.sin_addr);
    uint16_t server_port = ntohs(server_address.sin_port);

    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    struct conn connection;
    uint64_t session_id = generate_session_id();
    printf("session_id: %" PRIu64 "\n", session_id);
    init_conn(&connection, 2, 1, session_id);
    ssize_t sent_length = sendto(socket_fd, &connection, sizeof(connection), 0,
                                 (struct sockaddr *) &server_address, sizeof(server_address));
    if (sent_length < 0){
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
        fatal("expected con_acc packet");
    }
    if (con_acc.meta.session_id != connection.meta.session_id) {
        fatal("session_id mismatch");
    }
    printf("received con_acc packet\n");


}
void run_client_udpr(char const* host, uint16_t port) {

}

void run_client_tcp(char const* host, uint16_t port) {

}


void run_client(uint8_t protocol_type, char const* host, uint16_t port) {
    if (protocol_type == 1) {
        run_client_tcp(host, port);
    } else if (protocol_type == 2) {
        run_client_udp(host, port);
    } else if (protocol_type == 3) {
        run_client_udpr(host, port);
    } else {
        fatal("unknown protocol type");
    }
}

char* read_data() {
    char* buffer = malloc(1024);
    uint64_t free_space = 1024;
    uint64_t read_bytes = 0;

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