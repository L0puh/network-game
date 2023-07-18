#include "net.h"
#include <cstring>
#include <mutex>
#include <sys/socket.h>
#include <vector>

void print_board(char board[max_row][max_col]);
void handle_client(int sockfd, user_t usr);
void close_connection(int id, int sockfd);
Pos_t start();
Pos_t current_p, current_p2;
std::mutex mtx;
std::vector<connection_t> connections;

Pos_t move(char direction, int id){
    Pos_t pos_user = current_p;
    if (id != 1) {
        pos_user = current_p2;
    } 
    bool up = direction == 'w' && pos_user.y != 0;
    bool down = direction == 's' && pos_user.y != max_row-1;
    bool left = direction == 'a' && pos_user.x != 0;
    bool right = direction == 'd' && pos_user.x != max_col-1;
    int x = pos_user.x, y = pos_user.y;
    
    bool c_up=true, c_down=true, c_rigth=true, c_left=true;
    if (connections.size() > 1) {
        Pos_t current = current_p2;
        if (id == 1) {
            current = current_p;
        }
        //FIXME
        c_up = (y-1 != current.y && x != current.x); 
        c_down = (y+1 != current.y && x != current.x); 
        c_rigth = (y != current.y && x+1 != current.x);
        c_left = (y != current.y && x-1 != current.x); 
    }
    if (up && c_up) {
        pos_user.y--;
    } else if (down && c_down) {
        pos_user.y++;
    } else if (right && c_rigth) {
        pos_user.x++;
    } else if (left && c_left) {
        pos_user.x--;
    }

    return pos_user;
}

int main () {
    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        exit(1);
    freeaddrinfo(servinfo);
    if(listen(sockfd, 3) == -1){
        printf("ERROR: %s\n", strerror(errno));
        exit(1);
    }
    struct sockaddr_in their_addr;
    socklen_t size_addr = sizeof(their_addr);

    int id=0;
    while (true){
        int new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &size_addr);
        connection_t conn;
        id++;
        Pos_t cur_p; 
        Pos_t pos_me = start();
        if (id != 1) {
            current_p2 = pos_me;
           cur_p = current_p2;
        } else {
            current_p = pos_me;
            cur_p = current_p;
        }
        user_t usr = {.id = id, .pos_me = pos_me, .pos_usr = cur_p};
        send(new_sockfd, &usr, sizeof(usr), 0);

        conn.id = id; 
        conn.sockfd = new_sockfd;
        connections.push_back(conn);

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
void handle_client(int sockfd, user_t usr){
    int bytes_recv, bytes_sent;
    char direction;
    Pos_t pos = usr.pos_me;
    user_t user = usr;
    while((bytes_recv = recv(sockfd, &direction, sizeof(char), 0)) != 0){
        pos = move(direction, usr.id); 
        if (usr.id == 1) {
            current_p = pos;
        } else {
            current_p2 = pos;
        }
        usr.pos_me = pos;
        std::vector<connection_t>::iterator itr=connections.begin(); 
        for (; itr!=connections.end(); itr++){
               bytes_sent = send(itr->sockfd, &usr, sizeof(usr), 0 );
               if (bytes_sent == -1)
                   exit(1);
        }
        printf("[%d] x=%d; y=%d\n", usr.id, pos.x, pos.y);
    }
    if (bytes_recv == 0)
        close_connection(usr.id, sockfd);
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
                else {
                    char msg = 'q'; Pos_t pos;
                    send(itr->sockfd, &pos, sizeof(pos), 0);
                    send(itr->sockfd, &msg, sizeof(msg), 0);
                    itr++; 
                }
            } 
        }
}

