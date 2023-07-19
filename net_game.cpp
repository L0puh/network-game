#include "net.h"

Pos_t Game::start() {
    int x = std::rand(), y = std::rand();
    x = x % max_col + 1; 
    y = y % max_row + 1; 
    Pos_t pos = {.x = x, .y = y};
    return pos;
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
void Game::print_board(char board[max_row][max_col]){
    for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            printf("%c", board[row][colm]);
        }
        printf("%d\n", row);
    } 
}

void Game::create_board(){
   for (int row = 0; row < max_row; row++) {
        for (int colm = 0; colm < max_col; colm++){
            board[row][colm] = ' ';
        }
    }
}


