//
// Created by Jan Jagodziński on 29/03/2024.
//
#include <stdlib.h>

#include "packets.h"
#include "err.h"

void create_conn(struct conn * conn, uint8_t protocol_id, uint64_t net_sequence_length, uint64_t session_id) {
    if (conn == NULL) {
        fatal("conn is NULL");
    }
    conn->meta.packet_type_id = 1;
    conn->meta.session_id = session_id;
    conn->protocol_id = protocol_id;
    conn->net_sequence_length = net_sequence_length;
}