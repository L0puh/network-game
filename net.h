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

#include <fstream>
#include <thread>
#include <mutex>
#include <vector>
#include <termios.h> //getch


struct Pos_t{
    int x=0;
    int y=0;
};
struct User_t{
    int id;
    Pos_t pos;
};
struct connection_t{
    int id;
    int sockfd;
};


struct addrinfo* init_servinfo();

class Game {
    private:
        const static int  max_row = 10;
        const static int max_col = 20;
    public:
        void create_board();
    protected:
        char board[max_row][max_col];
        Pos_t prev_pos, prev_pos2;
        void print_board(char board[max_row][max_col]);
        Pos_t start();
        int getch();
        Pos_t move(char direction, Pos_t pos, int id);
        bool check_empty(Pos_t pos, Pos_t p_pos, char direction);

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
        void handle_recv(User_t cur_usr);
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
