#include "net.h"

struct addrinfo* init_servinfo(){
    struct addrinfo hints, *servinfo;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, "9000", &hints, &servinfo) == -1)
        exit(1);
    
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    return servinfo;
}
void Server::close_connection(int id, int sockfd) {
        printf("user %d is out\n", id);
        close(sockfd);
        {
            std::lock_guard<std::mutex> lock(mtx);
            std::vector<connection_t>::iterator itr = connections.begin();
            while (itr != connections.end()){
                if(itr->id == id)
                    connections.erase(itr);
            } 
        }
}

Server::Server(){
    id = 0;
    sockfd_listen = init_server();
}
Server::~Server(){
    close(sockfd_listen);
    exit(1);
}
int Server::add_connection(){
    int new_sockfd = accept(sockfd_listen, (struct sockaddr *)&their_addr, &size_addr);
    
    id++; 
    connection_t conn{.id = id, .sockfd = new_sockfd};
    connections.push_back(conn);

    return new_sockfd;
}

User_t Server::create_user(){
    Pos_t usr_pos = start();
    User_t usr{.id = id, .pos = usr_pos};
    for (auto itr = connections.begin(); itr!=connections.end(); itr++){
        send(itr->sockfd, &usr, sizeof(usr), 0);
    }
    return usr;
}
void Server::create_thread(){
    int new_sockfd = add_connection();
    User_t usr = create_user();
     
    std::thread th(&Server::handle_client, this, new_sockfd, usr);
    th.detach();
}
int Server::init_server(){
    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        exit(1);
    freeaddrinfo(servinfo);
    if(listen(sockfd, 3) == -1){
        printf("ERROR: %s\n", strerror(errno));
        exit(1);
    }
    return sockfd;
}
void Server::handle_client(int sockfd, User_t usr){
    int bytes_recv, bytes_sent;
    char direction;
    Pos_t pos = usr.pos;
    int id = usr.id;
    while((bytes_recv = recv(sockfd, &direction, sizeof(char), 0)) > 0){

        pos = move(direction, usr.pos, id); 
        usr.pos = pos;
        if (id == 1) {
            prev_pos = pos;
        } else {
            prev_pos2 = pos;
        }
        std::vector<connection_t>::iterator itr=connections.begin(); 
        for (;itr != connections.end(); itr++){
           bytes_sent = send(itr->sockfd, &usr, sizeof(usr), 0);
           if (bytes_sent == -1)
               exit(1);
        }
        printf("[%d] x=%d; y=%d\n", id, pos.x, pos.y);
    }
    if (bytes_recv == 0)
        close_connection(id, sockfd);
}
