#include "net.h"
#include <cstdio>

Client::Client(){
}
Client::~Client(){

}

int Client::init_client(){
    
    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        exit(1);
    freeaddrinfo(servinfo);
    
    return sockfd;
}
User_t Client::handle_start(){
    User_t usr;
    int bytes_recv;
    bytes_recv = recv(sockfd, &usr, sizeof(usr), 0);
    if (bytes_recv == -1 )
        exit(1);
    return usr;

}

User_t Client::add_user(){
    sockfd = init_client();
    User_t usr = handle_start(); 
    users.push_back(usr);
    board[usr.pos.y][usr.pos.x] = '*';
    print_board(board);
    return usr;
}
void Client::create_threads(){
    
    User_t usr = add_user();
    current_user = usr;
    std::thread recvth(&Client::handle_recv, this, usr);
    std::thread sendth(&Client::send_direction, this, usr);

    sendth.join();
    recvth.join();

}
void Client::handle_recv(User_t user){
    int bytes;
    Package pckg;
    while ((bytes = recv(sockfd, &pckg, sizeof(pckg), 0 )) > 0 ){
        if(pckg.user.id == user.id) {
            board[prev_pos.y][prev_pos.x] = ' ';
            board[pckg.user.pos.y][pckg.user.pos.x] = '*';
            current_user = pckg.user;
            prev_pos = pckg.user.pos;
        } else {
            if (pckg.hit)
                attack(pckg.hit_direction, pckg.user.pos, pckg.user.id);
            board[prev_pos2.y][prev_pos2.x] = ' ';
            board[pckg.user.pos.y][pckg.user.pos.x] = '@';
            prev_pos2 = pckg.user.pos;
            coop_user = pckg.user;
        }
        std::ostringstream s;
        s << current_user.HP;
        std::string msg(s.str());
        print_board(board, msg);
    }
}
void Client::send_direction(User_t cur_user){
    bool hit=false;
    char hit_direction;
    while (true){
        char direction = getch();
        if (direction == ' ') { //space to attack
            std::string msg = "which direction to shoot?";
            print_board(board, msg);
            hit_direction = getch();
            hit = attack(hit_direction, current_user.pos, current_user.id);
        }
        Package pckg{ .user = cur_user, .direction = direction, .hit_direction = hit_direction, .hit=hit };
        int bytes_sent = send(sockfd, &pckg, sizeof(pckg), 0);
        hit=false;
    }
}
