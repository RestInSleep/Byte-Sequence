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

}


#define BUFFER_SIZE 20

static uint16_t read_protocol(char *input) {
    if (strcmp(input, "tcp") == 0) {
        return 1;
    } else if (strcmp(input, "udp") == 0) {
        return 2;
    } else if (strcmp(input, "udpr") == 0) {
        return 3;
    } else {
        fatal("usage: %s <protocol ('tcp' / 'udp' / 'udpr')> <port>\n", "ppcbs");
    }
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
int main (int argc, char *argv[]) {
    srand(time(NULL));
    if (argc != 4) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <host> <port>  ...\n", argv[0]);
    }
    char const * host = argv[2];
    uint16_t port = read_port(argv[2]);
    struct sockaddr_in server_address = get_server_address(host, port);
    char const *server_ip = inet_ntoa(server_address.sin_addr);
    uint16_t server_port = ntohs(server_address.sin_port);


    // Create a socket.
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket");
    }
    struct conn connection;




}