#ifndef NET_H
#define NET_H
#include <cstdlib>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h> //close

#include <string>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h> //stderr
#include <cstring> //memset 
#include <string>

#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <termios.h> //getch
#include <sstream> // ostringstream
#define MAX_HP 100

struct Pos_t{
    int x=0;
    int y=0;
};
struct User_t{
    int id;
    Pos_t pos;
    bool hit=false;
    int HP=MAX_HP;
};
struct connection_t{
    int id;
    User_t user;
    int sockfd;
};
struct Attack_t {
    bool hit=false;
    char direction;
};


struct addrinfo* init_servinfo();

class Game {
    private:
        const static int  max_row = 15;
        const static int max_col = 50;
    public:
        void create_board();
    protected:
        char board[max_row][max_col];
        User_t current_user, coop_user;
        Pos_t prev_pos, prev_pos2;
        void print_board(char board[max_row][max_col], std::string = " ");
        Pos_t start();
        int getch();
        Pos_t move(char direction, Pos_t pos, int id);
        bool check_empty(Pos_t pos, Pos_t p_pos, char direction);
    protected:
        bool attack(char direction, Pos_t pos, int id);
        bool attack_up(Pos_t pos, int id);
        bool attack_down(Pos_t pos, int id );
        bool attack_left(Pos_t pos, int id);
        bool attack_right(Pos_t pos, int id);

};

class Client : public Game {
    private:
        std::vector<User_t> users;
        std::mutex mtx;
        int sockfd;
    public:
        Client();
        ~Client();
    private:
        void send_direction();
        User_t handle_start();
        void handle_recv();
        int init_client();
        User_t add_user();
    public:
        void create_threads();
};

class Server : public Game {
    private:
        std::mutex mtx;
        int sockfd_listen; 
        struct sockaddr_in their_addr;
        socklen_t size_addr = sizeof(their_addr);
        int id;
        std::vector<connection_t> connections;

    public:
        Server();
        ~Server();
    private:
        int init_server();
        void close_connection(int sockfd, int id);
        void handle_client(int sockfd, User_t usr);
        int add_connection();
        User_t create_user();
    public:
        void create_thread();

};

#endif
