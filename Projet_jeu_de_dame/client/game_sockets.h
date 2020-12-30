#ifndef _GAME_SOCKETS_H
#define _GAME_SOCKETS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "game_engine.h"

void fonc_perror(const char *msg);
// returns a connected socket or fails and exits
int socket_connect(char *host, char *port);
// sends data or fails
int socket_send(int csock, void *buffer, int len);
// receives data or fails
int socket_recv(int csock, void *buffer, int len);
void socket_close(int csock);
int recv_client(int csock, char *buffer);
int send_client(int csock, char *buffer);
int listen_socket(int port);
struct Client add_client(int ssock, int* nb_c, int* max_fd, FILE * flux);
void rmv_client(struct Client clients[MAX_CLIENT], int i_to_remove, int* nb_c);

// ==================================
// PARTIE CLIENT
// ==================================
void recv_msg( int sock, char buffer[] );
void send_msg( int sock, char buffer[]);
int recv_server(int sock, char buffer[]);
int send_server(int sock, char *buffer);

#endif

