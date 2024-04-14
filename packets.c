//
// Created by Jan Jagodziński on 29/03/2024.
//
#include <stdlib.h>

#include "packets.h"
#include "err.h"
#include <string.h>

struct vector {
    char * data;
    size_t size;
    size_t capacity;
};

struct vector* init_vector(size_t capacity) {
    struct vector* vec = malloc(sizeof(struct vector));
    if (vec == NULL) {
        fatal("malloc");
    }
    vec->data = malloc(capacity);
    if (vec->data == NULL) {
        fatal("malloc");
    }
    vec->capacity = capacity;
    vec->size = 0;
    return vec;
}

void push_back(struct vector* vec, char data) {
    if (vec->size == vec->capacity) {
        size_t new_capacity = vec->capacity * 2;
        char* new_data = realloc(vec->data, new_capacity);
        if (new_data == NULL) {
            fatal("realloc: input probably too large");
        }
        vec->data = new_data;
        vec->capacity = new_capacity;
    }
    vec->data[vec->size++] = data;
}

char pop_back(struct vector* vec) {
    if (vec->size == 0) {
        fatal("pop_back: vector is empty");
    }
    return vec->data[--vec->size];
}





void init_conn(struct conn* conn, uint8_t protocol_id, uint64_t net_sequence_length, uint64_t session_id) {
    if (conn == NULL) {
        fatal("conn is NULL");
    }
    conn->meta.packet_type_id = 1;
    conn->meta.session_id = htobe64(session_id);
    conn->protocol_id = protocol_id;
    conn->net_sequence_length = htobe64(net_sequence_length);
}

void init_con_acc(struct con_acc* con_acc, uint64_t session_id) {
    if (con_acc == NULL) {
        fatal("con_acc is NULL");
    }
    con_acc->meta.packet_type_id = 2;
    con_acc->meta.session_id = htobe64(session_id);
}

void init_con_rjt(struct con_rjt* con_rjt, uint64_t session_id) {
    if (con_rjt == NULL) {
        fatal("con_rjt is NULL");
    }
    con_rjt->meta.packet_type_id = 3;
    con_rjt->meta.session_id = htobe64(session_id);
}

uint8_t read_protocol(char *input) {
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
