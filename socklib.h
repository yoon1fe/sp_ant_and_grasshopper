// socklib.h
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h>
#include <strings.h>
// register functions to be implemented
int make_server_socket(int);
int make_server_socket_q(int, int);
int connect_to_server(char*, int);
