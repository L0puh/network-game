#ifndef NET_H
#define NET_H
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h> //close

#include <string>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h> //stderr
#include <cstring> //memset 

#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
struct Pos_t{
    int x=0;
    int y=0;
};
struct user_t{
    int id;

    Pos_t pos_me;
    Pos_t pos_usr;
};

struct connection_t{
    int id;
    int sockfd;
    Pos_t pos;
};
int const max_row=10;
int const max_col=20;
struct addrinfo* init_servinfo();

#endif
