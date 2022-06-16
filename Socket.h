#ifndef CONTAINER_SOCKET_H
#define CONTAINER_SOCKET_H

#include <iostream>
#include <cstring>
#include <strings.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_LENGTH 256
#define CLIENT_ARG_NUM 4
#define SERVER_ARG_NUM 3
#define SUCCESS 0
#define MAX_CONNECTIONS 3
#define SERVER_TYPE_ARG "server"
#define CLIENT_TYPE_ARG "client"

#define USAGE_ERR "system error: Usage: ./sockets client <port><terminal_command_to_run>.\nServer Usage: ./sockets server <port>\n"
#define READ_ERR "system error: Reading data failed.\n"
#define WRITE_ERR "system error: Writing data failed.\n"
#define CONNECT_ERR "system error: Connecting to socket failed.\n"
#define SOCKET_ERR "system error: Failed to create socket.\n"
#define HOSTNAME_ERR "system error: Failed to obtain hostname.\n"
#define BIND_ERR "system error: Failed to bind socket.\n"
#define ACCEPT_ERR "system error: Failed to accept connection.\n"
#define LISTEN_ERR "system error: Failed to listen.\n"
#define SOCKNAME_ERR "system error: Failed to get socketname.\n"
#define RUN_CMD_ERR "system error: Failed to run client's command.\n"

int establish(unsigned short port);
int call_socket(char *hostname, unsigned short port);
int read_data(int sock, char *buf, int n);
int write_data(int sock, char* buf, int n);
int getConnection(int sock);

[[noreturn]] int runServer(unsigned short port);
int runClient(unsigned short port, char* cmdToRun);

#endif //CONTAINER_SOCKET_H
