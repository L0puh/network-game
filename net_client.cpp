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
    std::thread recvth(&Client::handle_recv, this);
    std::thread sendth(&Client::send_direction, this);

    sendth.join();
    recvth.join();

}
void Client::handle_recv(){
    int bytes;
    User_t usr;
    Pos_t prev_pos = current_user.pos;
    while((bytes = recv(sockfd, &usr, sizeof(usr), 0)) > 0 ){
        if(usr.id == current_user.id) {
            board[prev_pos.y][prev_pos.x] = ' ';
            board[usr.pos.y][usr.pos.x] = '*';
            current_user = usr;
            prev_pos = usr.pos;
        } else {
            if (usr.hit){
                char direction;
                recv(sockfd, &direction, sizeof(char), 0);
                attack(direction, usr.pos, usr.id);
            } else {  
                board[prev_pos2.y][prev_pos2.x] = ' ';
                board[usr.pos.y][usr.pos.x] = '#';
                prev_pos2 = usr.pos;
            }
            coop_user = usr;
        }

        std::ostringstream s;
        s << current_user.HP;
        std::string msg(s.str());
        print_board(board, msg);
    }
}

void Client::send_direction(){
    while (true){
        char direction = getch();
        int bytes_sent = send(sockfd, &direction, sizeof(direction), 0);
        if (direction == ' ') { //space to attack
            std::string msg = "which direction to shoot?";
            print_board(board, msg);
            direction = getch();
            bool res = attack(direction, current_user.pos, current_user.id);
            Attack_t att{.hit = res, .direction = direction};
            bytes_sent = send(sockfd, &att, sizeof(att), 0);
        }
    }
}
