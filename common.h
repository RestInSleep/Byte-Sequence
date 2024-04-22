//
// initd by Jan Jagodziński on 29/03/2024.
//

#ifndef SIK_1_COMMON_H
#define SIK_1_COMMON_H

#include <inttypes.h>


#define MAX_PACKET_SIZE 64000
#define QUEUE_LENGTH 10


struct  __attribute__ ((__packed__)) metadata {
    uint8_t packet_type_id; // 1 - conn, 2 - con_acc, 3 - con_rjt, 4 - data, 5 - acc, 6 - rjt, 7 - rcvd
    uint64_t session_id;
};

struct  __attribute__ ((__packed__)) conn {
    struct metadata meta;
    uint8_t protocol_id;
    uint64_t net_sequence_length;
};

struct  __attribute__ ((__packed__)) con_acc {
    struct metadata meta;
};

struct  __attribute__ ((__packed__)) con_rjt {
    struct metadata meta;
};

struct  __attribute__ ((__packed__)) data {
    struct metadata meta;
    uint64_t net_packet_number;
    uint32_t net_packet_bytes;
    char data[];
};

struct  __attribute__ ((__packed__)) acc {
    struct metadata meta;
    uint64_t net_packet_number;
};

struct  __attribute__ ((__packed__)) rjt {
    struct metadata meta;
    uint64_t net_packet_number;
};

struct  __attribute__ ((__packed__)) rcvd {
    struct metadata meta;
};

void init_conn(struct conn* conn, uint8_t protocol_id, uint64_t net_sequence_length, uint64_t session_id);
void init_con_acc(struct con_acc* con_acc, uint64_t session_id);
void init_con_rjt(struct con_rjt* con_rjt, uint64_t session_id);
void init_data(struct data* data, uint64_t net_packet_number, uint32_t net_packet_bytes, char* buffer, uint64_t session_id);
void init_acc(struct acc* acc, uint64_t net_packet_number, uint64_t session_id);
void init_rjt(struct rjt* rjt, uint64_t net_packet_number, uint64_t session_id);
void init_rcvd(struct rcvd* rcvd, uint64_t session_id);
uint8_t read_protocol(char *input);
uint16_t read_port(char const *string);
void set_timeout(int socket_fd);
void unset_timeout(int socket_fd);
struct sockaddr_in create_address(uint16_t port);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t readn(int fd, void *buf, size_t count);


#endif //SIK_1_COMMON_H
