#include "net.h"
#include <cstring>
#include <mutex>
#include <sys/socket.h>
#include <vector>
void print_board(char board[max_row][max_col]);
void handle_client(int sockfd, int i);
std::mutex mtx;
struct connection_t{
    int id;
    int sockfd;
    Pos_t pos;
};
std::vector<connection_t> connections;


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
        conn.id = id; 
        conn.sockfd = new_sockfd;
        connections.push_back(conn);

        std::thread th(handle_client, new_sockfd, id);
        th.detach();
    }
}


void handle_client(int sockfd, int id){
    int bytes_recv, bytes_sent;
    
    Pos_t pos;
    char direction;
    while((bytes_recv = recv(sockfd, &pos, sizeof(pos), 0)) != 0){
        recv(sockfd, &direction, sizeof(direction), 0);
        for (std::vector<connection_t>::iterator itr=connections.begin(); itr!=connections.end(); itr++){
            if (itr->id != id) {
               bytes_sent = send(itr->sockfd, &pos, sizeof(pos), 0 );
               bytes_sent = send(itr->sockfd, &direction, sizeof(direction), 0);
               if (bytes_sent == -1)
                   exit(1);
            }
        }
        printf("id:%d - x=%d; y=%d\n", id, pos.x, pos.y);
    }
    if (bytes_recv == 0){
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
}

