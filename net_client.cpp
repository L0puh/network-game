#include "net.h"

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
    std::thread recvth(&Client::handle_recv, this, usr);
    std::thread sendth(&Client::send_direction, this, usr);

    sendth.join();
    recvth.join();

}
void Client::handle_recv(User_t cur_usr){
    int bytes;
    User_t usr;
    Pos_t prev_pos = cur_usr.pos, prev_pos2;
    while((bytes = recv(sockfd, &usr, sizeof(usr), 0)) > 0 ){
        if(usr.id == cur_usr.id) {
            board[prev_pos.y][prev_pos.x] = ' ';
            board[usr.pos.y][usr.pos.x] = '*';
            prev_pos = usr.pos;
        } else {
            board[prev_pos2.y][prev_pos2.x] = ' ';
            board[usr.pos.y][usr.pos.x] = '#';
            prev_pos2 = usr.pos;
        }
            print_board(board);
    }
}

void Client::send_direction(User_t cur_usr){
    while (true){
        char direction = getch();
        if (direction == ' ') { //space to attack
            std::string msg = "which direction to shoot?";
            print_board(board, msg);
            direction = getch();
            attack(direction, cur_usr.pos);
            continue;

        }
        int bytes_sent = send(sockfd, &direction, sizeof(direction), 0);
    }
}
