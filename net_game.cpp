#include "net.h"

Pos_t Game::start() {
    int x = std::rand(), y = std::rand();
    x = x % max_col + 1; 
    y = y % max_row + 1; 
    Pos_t pos = {.x = x, .y = y};
    return pos;
}

void Game::attack_up(Pos_t pos){
    for(int i=0; i != pos.y; i++) {
        board[i][pos.x] = '.';
    }
    print_board(board);
    for (int i =0; i!= pos.y; i++){
        board[i][pos.x] = ' ';
    }
}
void Game::attack_down(Pos_t pos){
    for(int i=pos.y; i != max_row; i++) {
        board[i][pos.x] = '.';
    }
    print_board(board);
    for(int i=pos.y; i != max_row; i++) {
        board[i][pos.x] = ' ';
    }
}
void Game::attack_right(Pos_t pos){

    for (int i = pos.x; i != max_col; i++ ){
        board[pos.y][i] = '.';
    }
    print_board(board);
    for (int i = pos.x; i != max_col; i++ ){
        board[pos.y][i] = ' ';
    }
}
void Game::attack_left(Pos_t pos){
    for (int i = pos.x; i != 0; i-- ){
        board[pos.y][i] = '.';
    }
    print_board(board);
    for (int i = pos.x; i != 0; i-- ){
        board[pos.y][i] = ' ';
    }
}
void Game::attack(char direction, Pos_t pos){
    switch(direction){
        case 'w':
            attack_up(pos);
            break;
        case 'd':
            attack_right(pos);
            break;
        case 's':
            attack_down(pos);
            break;
        case 'a':
            attack_left(pos);
            break;
    }
}

Pos_t Game::move(char direction, Pos_t pos, int id){
    
    bool up = direction == 'w' && pos.y != 0;
    bool down = direction == 's' && pos.y != max_row-1;
    bool left = direction == 'a' && pos.x != 0;
    bool right = direction == 'd' && pos.x != max_col-1;
    bool non_null;
    if (id == 1) {
        non_null = check_empty(pos, prev_pos2, direction);
    } else {
        non_null = check_empty(pos, prev_pos, direction);
    }
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

bool Game::check_empty(Pos_t pos, Pos_t p_pos, char direction){
    bool non_null;
    int y = pos.y, x = pos.x;
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
        printf("%d\n", row);
        if (row == 0) {
            printf("%s", msg.c_str());
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


