//
// Created by Jan Jagodziński on 29/03/2024.
//

#ifndef SIK_1_PACKETS_H
#define SIK_1_PACKETS_H
#include <inttypes.h>


struct metadata {
    uint8_t type_id;
    uint64_t session_id;
};

struct conn {
    struct metadata meta;
    uint8_t protocol_id;
    uint64_t net_sequence_length;
};

struct con_acc {
    struct metadata meta;
};

struct con_rjt {
    struct metadata meta;
};

struct data {
    struct metadata meta;
    uint64_t net_packet_number;
    uint32_t net_packet_bytes;
    char data[64000];
};

struct acc {
    struct metadata meta;
    uint64_t net_packet_number;
};

struct rjt {
    struct metadata meta;
    uint64_t net_packet_number;
};

struct rcvd {
    struct metadata meta;
};

#endif //SIK_1_PACKETS_H
