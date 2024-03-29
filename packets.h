//
// Created by Jan Jagodziński on 29/03/2024.
//

#ifndef SIK_1_PACKETS_H
#define SIK_1_PACKETS_H
#include <inttypes.h>


struct  __attribute__ ((__packed__)) metadata {
    uint8_t type_id;
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
    char data[64000];
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

struct conn * create_conn(uint8_t protocol_id, uint64_t net_sequence_length, uint64_t session_id);
struct con_acc * create_con_acc(uint64_t session_id);
struct con_rjt * create_con_rjt(uint64_t session_id);
struct data * create_data(uint64_t net_packet_number, uint32_t net_packet_bytes, char *data, uint64_t session_id);
struct acc * create_acc(uint64_t net_packet_number, uint64_t session_id);
struct rjt * create_rjt(uint64_t net_packet_number, uint64_t session_id);
struct rcvd * create_rcvd(uint64_t session_id);


#endif //SIK_1_PACKETS_H
