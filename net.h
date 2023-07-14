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

int const max_row=10;
int const max_col=30;
struct addrinfo* init_servinfo();
#endif
