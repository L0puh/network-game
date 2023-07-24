#include "../net.h"
#include <vector>

Pos_t Game::start() {
    int x = std::rand(), y = std::rand();
    x = x % max_col + 1; 
    y = y % max_row + 1; 
    Pos_t pos = {.x = x, .y = y};
    return pos;
}
std::vector<User_t>::iterator Game::return_coop_user(int id ){
    for (auto itr = users.begin(); itr != users.end(); itr++)
        if(id != itr->id) return itr;
    return users.begin();
}

bool Game::attack_up( User_t *usr){
    std::vector<User_t>::iterator itr = return_coop_user(usr->id);
    User_t coop_user = *itr;
    bool hitted=false;
    for(int i=0; i != usr->pos.y; i++) {
        board[i][usr->pos.x] = '.';
        if (coop_user.pos.y  == i && coop_user.pos.x == usr->pos.x){
            hitted = true;
        }
    }
    print_board(board);
    for (int i =0; i!= usr->pos.y; i++){
        board[i][usr->pos.x] = ' ';
    }
    return hitted;
}
bool Game::attack_down( User_t *usr){

    std::vector<User_t>::iterator itr = return_coop_user(usr->id);
    User_t coop_user = *itr;
    bool hitted=false;
    for(int i=0; i != max_row; i++) {
        board[i][usr->pos.x] = '.';
        if (coop_user.pos.y  == i && coop_user.pos.x == usr->pos.x){
            hitted = true;
        }
    }
    print_board(board);
    for (int i =0; i!= max_row; i++){
        board[i][usr->pos.x] = ' ';
    }
    return hitted;
}
bool Game::attack_right(User_t *usr ){

    std::vector<User_t>::iterator itr = return_coop_user(usr->id);
    User_t coop_user = *itr;
    bool hitted = false;
    for (int i = usr->pos.x; i != max_col; i++ ){
        board[usr->pos.y][i] = '.';
        if (coop_user.pos.y == i && coop_user.pos.x == usr->pos.x){
            hitted = true;
        }
    }
    print_board(board);
    for (int i = usr->pos.x; i != max_col; i++ ){
        board[coop_user.pos.y][i] = ' ';
    }
    return hitted;
}
bool Game::attack_left(User_t *usr){
    std::vector<User_t>::iterator itr = return_coop_user(usr->id);
    User_t coop_user = *itr;

    bool hitted = false;
    for (int i = usr->pos.x; i != 0; i-- ){
        board[usr->pos.y][i] = '.';
        if (coop_user.pos.x == i && usr->pos.y == coop_user.pos.y) {
            hitted = true;
        }
    }
    print_board(board);
    for (int i = usr->pos.x; i != 0; i-- ){
        board[usr->pos.y][i] = ' ';
    }
    return hitted;
}
bool Game::attack(char direction, User_t *usr){
    switch(direction){
        case 'w':
            return attack_up(usr);
        case 'd':
            return attack_right(usr);
        case 's':
            return attack_down(usr);
        case 'a':
            return attack_left(usr);
    }
    return false;
}

Pos_t Game::move(char direction, User_t *usr){
    Pos_t pos = usr->pos; 
    bool up = direction == 'w' && pos.y != 0;
    bool down = direction == 's' && pos.y != max_row-1;
    bool left = direction == 'a' && pos.x != 0;
    bool right = direction == 'd' && pos.x != max_col-1;
    bool non_null;
    non_null = check_empty(direction, usr);
    if (up && !non_null) {
        pos.y--;
    } else if (down && !non_null) {
        pos.y++;
    } else if (right && !non_null) {
        pos.x++;
    } else if (left && !non_null) {
        pos.x--;
    }

    return pos;
}
int Game::getch() {
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

bool Game::check_empty(char direction, User_t *usr){
    bool non_null;
    User_t coop_user;
    for (auto itr = connections.begin(); itr != connections.end(); itr++){
        if (usr->id != itr->pckg.user.id){
            coop_user = itr->pckg.user;
            break;
            }
    }
    Pos_t p_pos = coop_user.pos;
    int y = usr->pos.y, x = usr->pos.x;
    switch(direction){
        case 'w':
            return (y-1 == p_pos.y && x == p_pos.x);
        case 's':
            return (y+1 == p_pos.y && x == p_pos.x);
        case 'd':
            return (y == p_pos.y   && x+1 == p_pos.x);
        case 'a':
            return (y == p_pos.y   && x-1 == p_pos.x);
    }
    return true;
}
void Game::print_board(char board[max_row][max_col], std::string msg){
    system("clear");
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            printf("%c", board[row][colm]);
        }
        if (row == 0) {
            printf("%d : %s\n", row, msg.c_str());   

        }else {
            printf("%d\n", row);
        }
    } 
}

void Game::create_board(){
   for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            board[row][colm] = ' ';
        }
    }
}


