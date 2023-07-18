#include "net.h"
#include <cstring>
#include <mutex>
#include <sys/socket.h>
#include <vector>

void print_board(char board[max_row][max_col]);
void handle_client(int sockfd, User_t usr);
void close_connection(int id, int sockfd);
Pos_t start();

Pos_t prev_pos, prev_pos2;

std::mutex mtx;
std::vector<connection_t> connections;

bool check_non_null(Pos_t pos, Pos_t p_pos, char direction){
    bool non_null;
    int y = pos.y, x = pos.x;
    switch(direction){
        case 'w':
            return (y-1 == p_pos.y && x == p_pos.x);
        case 's':
            return (y+1 == p_pos.y && x == p_pos.x);
        case 'd':
            return (y == p_pos.y   && x+1 == p_pos.x);
        case 'a':
            return (y == p_pos.y   && x-1 == p_pos.x);
    }
    return true;
}
Pos_t move(char direction, Pos_t pos, int id){
    
    bool up = direction == 'w' && pos.y != 0;
    bool down = direction == 's' && pos.y != max_row-1;
    bool left = direction == 'a' && pos.x != 0;
    bool right = direction == 'd' && pos.x != max_col-1;

    bool non_null;
    if (id == 1) {
        non_null = check_non_null(pos, prev_pos2, direction);
    } else {
        non_null = check_non_null(pos, prev_pos, direction);
    }
    if (up && !non_null) {
        pos.y--;
    } else if (down && !non_null) {
        pos.y++;
    } else if (right && !non_null) {
        pos.x++;
    } else if (left && !non_null) {
        pos.x--;
    }

    return pos;
}
int init_server(){
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
int main () {
    struct sockaddr_in their_addr;
    socklen_t size_addr = sizeof(their_addr);
    int sockfd = init_server();

    int id=0;
    while (true){
        int new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &size_addr);
        connection_t conn;
        id++;

        conn.id = id; 
        conn.sockfd = new_sockfd;
        connections.push_back(conn);
        Pos_t usr_pos = start();
        User_t usr{.id = id, .pos = usr_pos};
        for (auto itr = connections.begin(); itr!=connections.end(); itr++){
            send(itr->sockfd, &usr, sizeof(usr), 0 );
        }

        std::thread th(handle_client, new_sockfd, usr);
        th.detach();
    }
}

Pos_t start() {
    int x = std::rand(), y = std::rand();
    x = x % max_col + 1; 
    y = y % max_row + 1; 
    Pos_t pos = {.x = x, .y = y};
    return pos;
}


void handle_client(int sockfd, User_t usr){
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

void close_connection(int id, int sockfd) {
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

