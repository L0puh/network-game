#include "net.h"
#include <netdb.h>
#include <sys/socket.h>
#include <termios.h>
#include <unistd.h>

char board[max_row][max_col];
std::mutex mtx;

void print_board(char board[max_row][max_col]);
void handle_recv(int sockfd);
void handle_send(int sockfd);
Pos_t move(Pos_t pos_user, char direction);

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
    // create a board
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

void handle_recv(int sockfd){
    int bytes_recv; 
    Pos_t pos, current_pos;
    char direction;
    while((bytes_recv = recv(sockfd, &pos, sizeof(pos), 0)) != -1){
        bytes_recv = recv(sockfd, &direction, sizeof(direction), 0);
        if (direction != 'q') {
            if (bytes_recv > 0 ){
                std::lock_guard<std::mutex> lock(mtx); 
                pos = move(pos, direction);
                current_pos = pos;
                board[pos.y][pos.x] = '#';
            }
            system("clear");
            print_board(board);
        } else {
            board[current_pos.y][current_pos.x] = ' ';
            system("clear");
            print_board(board);
            break;
        }
    }
}
void handle_send(int sockfd) {
    Pos_t pos_user, pos_user2;
    while (true){
        char direction = getch();
        system("clear");
        int bytes_sent = send(sockfd, &pos_user, sizeof(pos_user), 0);
        bytes_sent = send(sockfd, &direction, sizeof(direction), 0);
        if (bytes_sent > 0) {
            std::lock_guard<std::mutex> lock(mtx); 
            pos_user = move(pos_user, direction);
            board[pos_user.y][pos_user.x] = '*';
        }
        print_board(board);
    }
}

Pos_t move(Pos_t pos_user, char direction){
    bool up = direction == 'w' && pos_user.y != 0;
    bool down = direction == 's' && pos_user.y != max_row-1;
    bool left = direction == 'a' && pos_user.x != 0;
    bool right = direction == 'd' && pos_user.x != max_col-1;
    int x = pos_user.x, y = pos_user.y;
   
    if (up) {
        board[pos_user.y][pos_user.x] = ' ';
        pos_user.y--;
    } else if (down) {
        board[pos_user.y][pos_user.x] = ' ';
        pos_user.y++;
    } else if (right) {
        board[pos_user.y][pos_user.x] = ' ';
        pos_user.x++;
    } else if (left) {
        board[pos_user.y][pos_user.x] = ' ';
        pos_user.x--;
    }
    return pos_user;
}

