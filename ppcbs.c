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
#include <stdbool.h>
#include "err.h"
#include "common.h"
#include "protconst.h"


void udp_send_rjt(struct sockaddr_in *client_address, struct data* data, int socket_fd) {
    struct rjt rejection;
    init_rjt(&rejection, be64toh(data->net_packet_number), be64toh(data->meta.session_id));
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &rejection, sizeof(struct rjt), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "Client has not received rejection of connection info.\n");
    }
}


int udp_receive_conn(int socket_fd, struct conn *connection, struct sockaddr_in *client_address) {
    unset_timeout(socket_fd);
    ssize_t received_length;
    int flags = 0;
    socklen_t address_length = (socklen_t) sizeof(&client_address);
    received_length = recvfrom(socket_fd, connection, sizeof(struct conn), flags,
                               (struct sockaddr *) client_address, &address_length);
    if (received_length < 0) {
        fprintf(stderr, "Did not receive connection.\n");
        return -1;
    }
    if (received_length != sizeof(struct conn)) {
        fprintf(stderr, "received_length != sizeof(struct conn) in function udp_receive_conn.");
        return -1;
    }
    if (connection->meta.packet_type_id != 1) {
        if (connection->meta.packet_type_id == 4) {
            struct data *data = (struct data *) connection;
            udp_send_rjt(client_address, data, socket_fd);
        }
        fprintf(stderr, "Server expects connection packet first!\n");
        return -1;
    }
    if (connection->protocol_id == 1) {
        fprintf(stderr, "Incoming tcp connection to udp server!\n");
        return -1;
    }
    if (connection->protocol_id != 2 && connection->protocol_id != 3) {
        fprintf(stderr, "Unknown connection to udp server!\n");
        return -1;
    }
    return 1;
}

ssize_t udp_send_con_acc(int socket_fd, struct sockaddr_in *client_address, uint64_t session_id) {
    struct con_acc con_acc;
    init_con_acc(&con_acc, session_id);
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &con_acc, sizeof(struct con_acc), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
   return sent;
}

ssize_t udp_send_con_rjt(int socket_fd, struct sockaddr_in *client_address, uint64_t session_id) {
    struct con_rjt con_rjt;
    init_con_rjt(&con_rjt, session_id);
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &con_rjt, sizeof(struct con_rjt), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    return sent;
}

/*
 * 1 - everything is ok
 * 0 - something wrong, but client should be still served
 * -1 - something wrong, connection should be closed
 * -2 - timeout - connection should be closed if udp, if udpr then retransmit
 * -3 - received packet which had already been received - if udp, then close connection,
 *  if udpr then continue
 *
 */
int udp_receive_data_packet(int socket_fd, struct data *data, uint64_t session_id, uint64_t *currently_received, uint64_t *expected_packet_number) {
    ssize_t received_length;
    int flags = 0;
    struct sockaddr_in incoming_address;
    socklen_t address_length = (socklen_t) sizeof(incoming_address);
    received_length = recvfrom(socket_fd, data, sizeof(struct data) + MAX_PACKET_SIZE, flags,
                               (struct sockaddr *) &incoming_address, &address_length);
    if (received_length < 0) {
        if (errno == EAGAIN) {
            fprintf(stderr, "Timeout!\n");
            return -2;
        } else {
        fprintf(stderr, "recvfrom failed in function udp_receive_data_packet.\n");
        }
        return -1;
    }

    if (be64toh(data->meta.session_id) != session_id) {
        if (data->meta.packet_type_id == 1) {
            struct conn *connection = (struct conn *) data;
            fprintf(stderr, "Already serving a connection!\n");
            udp_send_con_rjt(socket_fd, &incoming_address, be64toh(connection->meta.session_id));
        } else if (data->meta.packet_type_id == 4) {
            fprintf(stderr, "Data with wrong session_id!\n");
            udp_send_rjt(&incoming_address, data, socket_fd);
        }
        return 0;
    }

    if (data->meta.packet_type_id != 4) {
        fprintf(stderr, "Server expects data packet!\n");
        if (data->meta.packet_type_id == 1) {
           fprintf(stderr, "Received connection packet which had already been received.\n");
           return -3;
        }
        return -1;
    }

    if((uint32_t)received_length != sizeof(struct data) + be32toh(data->net_packet_bytes)) {
        fprintf(stderr, "Received_length != sizeof(struct data) + be32toh(data->net_packet_bytes).\n");
        udp_send_rjt(&incoming_address, data, socket_fd);
        return -1;
    }

    if (be64toh(data->net_packet_number) != *expected_packet_number) {
        if (be64toh(data->net_packet_number) < *expected_packet_number) {
            fprintf(stderr, "Received data packet with number %" PRIu64 " which had already been received.\n", be64toh(data->net_packet_number));
            return -3;
        }
        fprintf(stderr, "Wrong packet number %lu! \n", be64toh(data->net_packet_number));
        udp_send_rjt(&incoming_address, data, socket_fd);
        return -1;
    }
    fprintf(stderr, "Received data packet with number %lu \n", *expected_packet_number);
    fprintf(stderr, "Received data packet with size %." PRIu32 "\n", be32toh(data->net_packet_bytes));
    ssize_t wrote = write(STDOUT_FILENO, data->data, be32toh(data->net_packet_bytes));
    if (wrote < 0) {
        fprintf(stderr, "Write failed.\n");
        return -1;
    }
    fflush(stdout);
    *currently_received += be32toh(data->net_packet_bytes);
    *expected_packet_number += 1;
    return 1;
}

void udp_send_rcvd(int socket_fd, struct sockaddr_in *client_address, uint64_t session_id) {
    struct rcvd received;
    int flags = 0;
    init_rcvd(&received, session_id);
    ssize_t sent = sendto(socket_fd, &received, sizeof(struct rcvd), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));

    if (sent != sizeof(struct rcvd)) {
        fprintf(stderr, "partial / failed write\n");
    }
    fprintf(stderr, "received all data\n");
}


static void udp_server_no_retransmit_recv(uint64_t session_id, struct sockaddr_in* client_address,
                                          uint64_t sequence_length, int socket_fd, char *buffer) {

    if (udp_send_con_acc(socket_fd, client_address, session_id) < 0) {
        fprintf(stderr, "con_acc could not be sent, closing connection...\n");
        return;
    }
    uint64_t currently_received = 0;
    uint64_t expected_packet_number = 0;
    while (currently_received < sequence_length) {
        int received = udp_receive_data_packet(socket_fd, (struct data *) buffer, session_id, &currently_received, &expected_packet_number);
        if (received < 0) {
            fprintf(stderr, "data could not be received, closing connection...\n");
            return;
        }
    }
    udp_send_rcvd(socket_fd, client_address, session_id);
}

int udp_send_acc(int socket_fd, struct sockaddr_in *client_address, uint64_t expected_packet_number, uint64_t session_id) {
    struct acc acc;
    init_acc(&acc, expected_packet_number, session_id);
    int flags = 0;
    ssize_t sent = sendto(socket_fd, &acc, sizeof(struct acc), flags,
                          (struct sockaddr *) client_address, (socklen_t) sizeof(*client_address));
    if (sent < 0) {
        fprintf(stderr, "Client has not received acceptance of data packet.\n");
        return -1;
    }
    else if (sent != sizeof(struct acc)) {
        fprintf(stderr, "partial / failed write\n");
        return -1;
    }
    return 1;
}





static void udp_server_retransmit_recv(uint64_t session_id, struct sockaddr_in client_address,
                                       uint64_t sequence_length, int socket_fd, char *buffer) {

    /*
     * send con_acc
     * receive data
     * send acc
     */
    uint64_t expected_packet_number = 0;
    uint64_t currently_received = 0;
    int current_retransmissions = 0;

    udp_send_con_acc(socket_fd, &client_address, session_id);
    while(currently_received < sequence_length) {
       int result =  udp_receive_data_packet(socket_fd, (struct data *) buffer, session_id, &currently_received, &expected_packet_number);
       if (result == -1) {
           fprintf(stderr, "data could not be received, closing connection...\n");
           return;
       }
       if (result == -3) {
           continue;
       }
       if (result == 1) {
           sleep(2);
           udp_send_acc(socket_fd, &client_address, expected_packet_number - 1, session_id);
           current_retransmissions = 0;
       }
       if (result == -2) {
           if (current_retransmissions == MAX_RETRANSMITS) {
               fprintf(stderr, "too many retransmissions, closing connection...\n");
               return;
           }
           if (expected_packet_number == 0) {
               udp_send_con_acc(socket_fd, &client_address, session_id);
           }
           else {
               udp_send_acc(socket_fd, &client_address, expected_packet_number - 1, session_id);
           }
              current_retransmissions++;
         }
    }
    udp_send_rcvd(socket_fd, &client_address, session_id);
}


static void udp_server_run(uint16_t port, char *receive_buffer) {
    int socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        syserr("cannot create a socket, cannot run server\n");
    }
    struct sockaddr_in server_address = create_address(port);
    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
    while (true) {
        struct conn connection;
        struct sockaddr_in client_address;
        if (udp_receive_conn(socket_fd, &connection, &client_address) < 0) {
            continue;
        }
        fprintf(stderr, "received connection\n");
        set_timeout(socket_fd);
        uint64_t session_id = be64toh(connection.meta.session_id);
        uint64_t sequence_length = be64toh(connection.net_sequence_length);
        uint8_t protocol_id = connection.protocol_id;
        if (protocol_id == 2) {
            udp_server_no_retransmit_recv(session_id, &client_address, sequence_length, socket_fd, receive_buffer);
        } else if (protocol_id == 3) {
            fprintf(stderr, "retransmit\n");
            udp_server_retransmit_recv(session_id, client_address, sequence_length, socket_fd, receive_buffer);
        } else {
            fprintf(stderr, "unknown protocol!\n");
        }
    }
}

/*
 * TCP
 */
int tcp_receive_conn(int client_fd, uint64_t* session_id, uint64_t* sequence_length) {
    struct conn connection;
    ssize_t received_length = readn(client_fd, &connection, sizeof(struct conn));
    if (received_length < 0) {
        fprintf(stderr, "Did not receive connection.\n");
        return -1;
    }
    if (received_length != sizeof(struct conn)) {
        fprintf(stderr, "received_length != sizeof(struct conn)");
        return -1;
    }
    if (connection.meta.packet_type_id != 1) {
        fprintf(stderr, "Server expects connection packet first!\n");
        return -1;
    }
    if (connection.protocol_id == 2 || connection.protocol_id == 3) {
        fprintf(stderr, "Incoming udp connection to tcp server!\n");
        return -1;
    }
    if (connection.protocol_id != 1) {
        fprintf(stderr, "unknown connection to tcp server!\n");
        return -1;
    }
    *session_id = be64toh(connection.meta.session_id);
    *sequence_length = be64toh(connection.net_sequence_length);
    return 1;
}

int tcp_send_con_acc(int client_fd, uint64_t session_id) {
    struct con_acc con_acc;
    init_con_acc(&con_acc, session_id);
    ssize_t sent = writen(client_fd, &con_acc, sizeof(struct con_acc));
    if (sent < 0) {
        fprintf(stderr, "con_acc could not be sent, closing connection...\n");
        return -1;
    }
    return 1;
}

int tcp_send_rjt(int client_fd, uint64_t session_id, uint64_t packet_number) {
    struct rjt rjt;
    init_rjt(&rjt, packet_number, session_id);
    ssize_t sent = writen(client_fd, &rjt, sizeof(struct rjt));
    if (sent < 0) {
        fprintf(stderr, "rjt could not be sent, closing connection...\n");
        return -1;
    }
    return 1;
}

int tcp_send_rcvd(int client_fd, uint64_t session_id) {
   fprintf(stderr, "sending rcvd\n");
    struct rcvd rcvd;
    init_rcvd(&rcvd, session_id);
    ssize_t sent = writen(client_fd, &rcvd, sizeof(struct rcvd));
    if (sent < 0) {
        fprintf(stderr, "rcvd could not be sent, closing connection...\n");
        return -1;
    }
    fprintf(stderr, "rcvd sent\n");
    return 1;
}

    int tcp_receive_data_packet(int client_fd, uint64_t session_id, uint64_t *currently_received,
                                char *receive_buffer, uint64_t *expected_packet_number) {
        struct data data;
        ssize_t received_length = readn(client_fd, &data, sizeof(struct data));
        if (received_length < 0) {
            fprintf(stderr, "Did not receive data packet.\n");
            return -1;
        }
        if (received_length != sizeof(struct data)) {
            fprintf(stderr, "received_length != sizeof(struct data)\n");
            return -1;
        }
        if (data.meta.packet_type_id != 4) {
            fprintf(stderr, "Server expects data packet! closing connection... \n");
            return -1;
        }
        if (be64toh(data.meta.session_id) != session_id) {
            fprintf(stderr, "data with wrong session_id!\n");
            return -1;
        }
        if (be64toh(data.net_packet_number) != *expected_packet_number) {
            fprintf(stderr, "wrong packet number! %lu \n", be64toh(data.net_packet_number));
            return -1;
        }
        ssize_t data_length = be32toh(data.net_packet_bytes);
        ssize_t received_data_length = readn(client_fd, receive_buffer, data_length);
        if (received_data_length < 0) {
            if (errno == EAGAIN) {
                fprintf(stderr, "timeout\n");
            } else {
                fprintf(stderr, "readn failed\n");
            }
            fprintf(stderr, "Did not receive data.\n");
            return -1;
        }
        if (received_data_length != data_length) {
            fprintf(stderr, "received_data_length != data_length");
            return -1;
        }
        ssize_t wrote = write(STDOUT_FILENO, receive_buffer, data_length);
        if (wrote < 0) {
            fprintf(stderr, "write failed\n");
            return -1;
        }
        if (wrote != data_length) {
            fprintf(stderr, "incomplete write. closing connection... \n");
            return -1;
        }
        fflush(stdout);
        *currently_received += data_length;
        *expected_packet_number += 1;
        return 1;
    }



void tcp_server_recv(int socket_fd, char * receive_buffer) {
    struct sockaddr_in client_address;
    int client_fd = accept(socket_fd, (struct sockaddr *) &client_address,
                           &((socklen_t){sizeof(client_address)}));
    if (client_fd < 0) {
        fprintf(stderr, "accept failed\n");
        return;
    }
    fprintf(stderr, "accepted connection\n");
    set_timeout(client_fd);
    uint64_t session_id;
    uint64_t sequence_length;
    uint64_t currently_received = 0;
    uint64_t expected_packet_number = 0;
    if (tcp_receive_conn(client_fd, &session_id, &sequence_length) < 0) {
        return;
    }
    fprintf(stderr, "received connection with session_id %" PRIu64 " and sequence_length %" PRIu64 "\n", session_id, sequence_length);
    if (tcp_send_con_acc(client_fd, session_id) < 0) {
        return;
    }
    while(currently_received < sequence_length) {
        if (tcp_receive_data_packet(client_fd, session_id, &currently_received, receive_buffer, &expected_packet_number) < 0) {
            tcp_send_rjt(client_fd, session_id, expected_packet_number);
            return;
        }
    }
    fprintf(stderr, "received all data\n");
    tcp_send_rcvd(client_fd, session_id);
    close(client_fd);
}



static void tcp_server_run(uint16_t port, char *receive_buffer) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    //TODO
    if (socket_fd < 0) {
        syserr("cannot create a socket, cannot run server\n");
    }
    struct sockaddr_in server_address = create_address(port);
    if (bind(socket_fd, (struct sockaddr *) &server_address, (socklen_t) sizeof(server_address)) < 0) {
        syserr("bind");
    }
    if (listen(socket_fd, QUEUE_LENGTH) < 0) {
        syserr("listen");
    }
    while(true) {
        unset_timeout(socket_fd);
        tcp_server_recv(socket_fd, receive_buffer);
    }
}

void run_server(uint8_t protocol_type, uint16_t port) {
    static char receive_buffer[sizeof (struct data) + MAX_PACKET_SIZE];
    if (protocol_type == 1) {
        tcp_server_run(port, receive_buffer);
    }
    if (protocol_type == 2 || protocol_type == 3) {
        udp_server_run(port, receive_buffer);
    }
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        fatal("usage: %s <protocol ('tcp' / 'udp')> <port>\n", argv[0]);
    }
    run_server(read_protocol(argv[1]), read_port(argv[2]));
    return 0;
}