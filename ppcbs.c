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
#include "err.h"
#include "packets.h"

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

int main (int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", argv[0]);
    }
    int protocol_type = read_protocol(argv[1]); // 1- tcp, 2 - udp
    uint16_t port = read_port(argv[2]);








}