#include "net.h"
#include <cstring>
#include <sys/socket.h>
void print_board(char board[max_row][max_col]);
void handle_client(int sockfd, int i);

char board[max_row][max_col];

int main () {
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            board[row][colm] = ' ';
        }
    }

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
    int i=0;
    while (true){
        int new_sockfd = accept(sockfd, (struct sockaddr *)&their_addr, &size_addr);
        char msg;
        if (i == 0){
            board[0][0]='*'; // player
            i++;
        } else {
            board[0][0]='#'; // player
        }
        std::thread th(handle_client, new_sockfd, i);
        th.detach();
    }
}


void print_board(char board[max_row][max_col]){
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            printf("%c", board[row][colm]);
        }
        printf("%d\n", row);
    } 
}

void handle_client(int sockfd, int i){
    int index=0, c_index=0, bytes_recv;
    char msg;
    while((bytes_recv = recv(sockfd, &msg, sizeof(msg), 0)) != 0){
        system("clear");
        printf("recv: %d bytes\n", bytes_recv);
        c_index=index;
        index++;
        if (i == 0) {
            board[0][index] = '*';
            if (board[0][c_index] != '#'){
                board[0][c_index]= ' '; //clear last one
            }
        } else {
            board[0][index] = '#';
            if (board[0][c_index] != '*'){
                board[0][c_index]= ' '; 
            }
        }
        print_board(board);
    }
}
