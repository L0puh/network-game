#include "net.h"
#include <netdb.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>
#include <cstdlib>


void print_board(char board[max_row][max_col]);
void handle_recv(int sockfd);
void handle_send(int sockfd);
user_t handle_start(int sockfd);

Pos_t move(Pos_t pos_user, char direction);

char board[max_row][max_col];
std::mutex mtx;
user_t init;

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

int main () {
   for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            board[row][colm] = ' ';
        }
    }

    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
        exit(1);
    freeaddrinfo(servinfo);
    

    init = handle_start(sockfd); 
  
    board[init.pos_me.y][init.pos_me.x] = '*';

    print_board(board);

    std::thread recvth(handle_recv, sockfd);
    std::thread sendth(handle_send, sockfd);

    recvth.join();
    sendth.join();
}

void print_board(char board[max_row][max_col]){
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            printf("%c", board[row][colm]);
        }
        printf("%d\n", row);
    } 
}

user_t handle_start(int sockfd){
    int bytes_recv;
    user_t usr;
    bytes_recv = recv(sockfd, &usr, sizeof(usr), 0);
    return usr;

}

void handle_send(int sockfd) {
    while (true){
        char direction = getch();
        board[init.pos_me.y][init.pos_me.x] = ' ';
        int bytes_sent = send(sockfd, &direction, sizeof(direction), 0);
    }
}


void handle_recv(int sockfd){
    int bytes_recv;
    user_t usr, old_usr=init;
    while((bytes_recv = recv(sockfd, &usr, sizeof(usr), 0)) != -1) {

        if (bytes_recv > 0 ){
            std::lock_guard<std::mutex> lock(mtx); 
            if (init.id == usr.id) {
                board[usr.pos_me.y][usr.pos_me.x] = '*';
                board[usr.pos_usr.y][usr.pos_usr.y] = 'x';
                init = usr;
            }
            else {
                board[old_usr.pos_me.y][old_usr.pos_me.x] = ' ';
                board[usr.pos_me.y][usr.pos_me.x] = 'x';
                board[usr.pos_usr.y][usr.pos_usr.y] = '*';
                old_usr = usr;
            }
        }
        system("clear");
        print_board(board);
        

    }
}

