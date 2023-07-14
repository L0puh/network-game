#include "net.h"
void print_board(char board[max_row][max_col]);

int main () {
    char board[max_row][max_col];
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            board[row][colm] = ' ';
        }
    }
    
    board[0][0]='*'; // player

    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        exit(1);
    freeaddrinfo(servinfo);

    struct sockaddr_in their_addr;
    socklen_t size_addr = sizeof(their_addr);
    int index=0, c_index=0;
    while (true){
        char msg;
        int bytes_recv = recvfrom(sockfd, &msg, sizeof(msg), 0, 
                (struct sockaddr*)&their_addr, &size_addr);
        system("clear");
        printf("recv: %d bytes\n", bytes_recv);
        c_index=index;
        index++;
        board[0][index] = '*';
        board[0][c_index]= ' '; //clear last one
        print_board(board);
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
