#include "net.h"
#include <netdb.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
#include <cstdlib>


void print_board(char board[max_row][max_col]);
void handle_player(int sockfd, User_t usr);
void handle_me(int sockfd, User_t usr);
void handle_recv(int sockfd, User_t cur_usr);

void send_direction(int sockfd);
User_t handle_start(int sockfd);
std::vector<User_t> users;
char board[max_row][max_col];
std::mutex mtx;

int getch() {
   struct termios oldt, newt;
   int ch;
   tcgetattr( 0, &oldt );
   newt = oldt;
   newt.c_lflag &= ~( ICANON | ECHO );
   tcsetattr( 0, TCSANOW, &newt );
   ch = getchar();
   tcsetattr(0, TCSANOW, &oldt );
   return ch;
}
int init_client(){
    
    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        exit(1);
    freeaddrinfo(servinfo);
    
    return sockfd;
}
int main () {
   for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            board[row][colm] = ' ';
        }
    }

    int sockfd = init_client();
    User_t usr = handle_start(sockfd); 
    users.push_back(usr);
    board[usr.pos.y][usr.pos.x] = '*';
    print_board(board);

    std::thread recvth(handle_recv, sockfd, usr);
    std::thread sendth(send_direction, sockfd);

    sendth.join();
    recvth.join();
}

void print_board(char board[max_row][max_col]){
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            printf("%c", board[row][colm]);
        }
        printf("%d\n", row);
    } 
}

User_t handle_start(int sockfd){
    User_t usr;
    int bytes_recv;
    bytes_recv = recv(sockfd, &usr, sizeof(usr), 0);
    if (bytes_recv == -1 )
        exit(1);
    return usr;

}

void handle_recv(int sockfd, User_t cur_usr){
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
            system("clear");
            print_board(board);
    }
}

void send_direction(int sockfd){
    while (true){
        char direction = getch();
        int bytes_sent = send(sockfd, &direction, sizeof(direction), 0);
    }
}
