#include "../net.h"

int main () {
   Server server; 
   server.create_board();
   while(true){
       server.create_thread();
   }
}




