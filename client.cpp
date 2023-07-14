#include "net.h"
#include <netdb.h>
#include <termios.h>
#include <unistd.h>

int getch() {
   struct termios oldt,
   newt;
   int ch;
   tcgetattr( STDIN_FILENO, &oldt );
   newt = oldt;
   newt.c_lflag &= ~( ICANON | ECHO );
   tcsetattr( STDIN_FILENO, TCSANOW, &newt );
   ch = getchar();
   tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
   return ch;
}

int main () {
    struct addrinfo *servinfo = init_servinfo();
    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    freeaddrinfo(servinfo);
    while (true){
        char word = getch();
        int bytes_sent = sendto(sockfd, &word, sizeof(word), 0, 
                servinfo->ai_addr, servinfo->ai_addrlen);
    }
}
