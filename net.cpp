#include "net.h"
struct addrinfo* init_servinfo(){
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, "9000", &hints, &servinfo) == -1)
        exit(1);
    
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    return servinfo;
}
