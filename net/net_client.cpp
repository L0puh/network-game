#include "../net.h"

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
    std::thread recvth(&Client::handle_recv, this, &usr);
    std::thread sendth(&Client::send_direction, this, &usr);

    sendth.join();
    recvth.join();

}
std::vector<User_t>::iterator Client::remove_move(int id){
    for (auto itr = users.begin(); itr != users.end(); itr++){
        if (itr->id == id) {
            board[itr->pos.y][itr->pos.x] = ' ';
            return itr;
        }
    }
    return users.begin();
}
bool Client::user_exsist(int id){
    for (std::vector<User_t>::iterator itr = users.begin(); itr != users.end(); itr++){
        if (id == itr->id){
            return true;
        }
    }
    return false;
}
std::vector<User_t>::iterator Client::get_user(int id){
    for(auto itr=users.begin(); itr!= users.end(); itr++){
        if (id == itr->id){
            return itr;
        }
    }
    return users.begin();
}
void Client::handle_recv(User_t *user){
    int bytes;
    Package_t pckg;
    while ((bytes = recv(sockfd, &pckg, sizeof(pckg), 0 )) > 0 ){
        if(pckg.user.id == user->id) {
            std::vector<User_t>::iterator itr = remove_move(user->id);
            board[pckg.user.pos.y][pckg.user.pos.x] = '*';
            *user = pckg.user;
            *itr = pckg.user;
        } else {
            if (!user_exsist(pckg.user.id)){
                users.push_back(pckg.user);
            }
            if (pckg.hit){
                attack(pckg.hit_direction, &pckg.user);
                std::vector<User_t>::iterator usr = get_user(pckg.user.id);
                usr->HP = pckg.user.HP;
            } else {
                std::vector<User_t>::iterator itr= remove_move(pckg.user.id);
                board[pckg.user.pos.y][pckg.user.pos.x] = '@';
                *itr = pckg.user;
            }
        }
        print_board(board, get_HP(user));
    }
}
void Client::send_direction(User_t *user){
    bool hit=false;
    char hit_direction;
    while (true){
        char direction = getch();
        if (direction == ' ') { //space to attack
            std::string msg = "which direction to shoot?";
            print_board(board, msg);
            hit_direction = getch();
            hit = attack(hit_direction, user);
        }
        Package_t pckg{ .user = *user, .direction = direction, .hit_direction = hit_direction, .hit=hit };
        int bytes_sent = send(sockfd, &pckg, sizeof(pckg), 0);
        hit=false;
    }
}
