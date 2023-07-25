#include "../net.h"

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
        if (usr.id == itr->id){
            itr->pckg.user = usr;
            printf("user(%d) is connected ", itr->id);
            send(itr->sockfd, &usr, sizeof(usr), 0);
        } else {
            for (auto itr2 = connections.begin(); itr2 != connections.end(); itr2++){
                if (itr2->id != usr.id){
                    send(itr->sockfd, &usr, sizeof(usr), 0);
                    send(itr->sockfd, &itr2->pckg, sizeof(itr2->pckg), 0);
                }
            }
        }
        
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


void Server::handle_client(int sockfd, User_t cur_user){
    int bytes;
    Package_t pckg;
    Pos_t pos = cur_user.pos;
    while ((bytes = recv(sockfd, &pckg, sizeof(pckg), 0)) > 0 ){
        if(!pckg.hit){
            pos = move(pckg.direction, &cur_user);
            cur_user.pos = pos;
            pckg.user = cur_user;
        }
            
        std::vector<connection_t>::iterator itr=connections.begin(); 
        for (;itr != connections.end(); itr++){
            if (pckg.hit && itr->id != cur_user.id){
                itr->pckg.user.HP--;
                bytes = send(itr->sockfd, &itr->pckg, sizeof(pckg), 0);
                continue;
            }
            if (pckg.user.id == itr->id) {
                itr->pckg=pckg;
            }
            bytes = send(itr->sockfd, &pckg, sizeof(pckg), 0);
        }
        printf("[%d] x=%d; y=%d\n", id, pos.x, pos.y);
    }
    if (bytes == 0)
        close_connection(id, sockfd);
}

