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
    int id=0;
    Pos_t pos;
    int HP=MAX_HP;
};
struct Package_t{

    User_t user; 
    char direction;
    char hit_direction=' ';
    bool hit=false;

};
struct connection_t{
    int id;
    int sockfd;
    Package_t pckg;
};



struct addrinfo* init_servinfo();

class Game {
    private:
        const static int  max_row = 15;
        const static int max_col = 50;
    public:
        void create_board();
    protected:
        std::vector<connection_t> connections;
        std::vector<User_t> users;
        char board[max_row][max_col];
    protected:
        void print_board(char board[max_row][max_col], std::string = " ");
        Pos_t start();
        int getch();
        Pos_t move(char direction, User_t *usr);
        bool check_empty(char direction, User_t *user);
        std::vector<User_t>::iterator return_coop_user(int id);
    protected:
        bool attack(char direction, User_t *user);
        bool attack_up(User_t *user);
        bool attack_down(User_t *user);
        bool attack_left(User_t *user);
        bool attack_right(User_t *user);

};

class Client : public Game {
    private:
        std::mutex mtx;
        int sockfd;
    public:
        Client();
        ~Client();
    private:
        bool user_exsist(int id);
        std::vector<User_t>::iterator remove_move(int id);
        void send_direction(User_t *user);
        User_t handle_start();
        void handle_recv(User_t *user);
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
        int id=0;

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
