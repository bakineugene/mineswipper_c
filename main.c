#include <time.h>
#include <stdlib.h>

#define SCREEN_X 9
#define SCREEN_Y 9

#include "colours.h"
#include "sdl_renderer.h"

bool running = true;

char MINE = 11;

enum Direction {
    DIR_CLICK = 0,
};


void open_board(char screen[SCREEN_X][SCREEN_Y], char board[SCREEN_X][SCREEN_Y], char x, char y) {
    if (x < 0 || x >= SCREEN_X || y < 0 || y >= SCREEN_Y) return;

    if (board[x][y] > 0 && board[x][y] <= 9) {
        screen[x][y] = board[x][y];
        return;
    }
    if (board[x][y] == MINE) {
        memcpy(screen, board, SCREEN_X * SCREEN_Y * sizeof(char));
        return;
    }
    if (board[x][y] == screen[x][y]) {
        return;
    }

    screen[x][y] = board[x][y];

    open_board(screen, board, x, y - 1);
    open_board(screen, board, x - 1, y);
    open_board(screen, board, x, y + 1);
    open_board(screen, board, x + 1, y);

    open_board(screen, board, x + 1, y + 1);
    open_board(screen, board, x - 1, y + 1);
    open_board(screen, board, x + 1, y - 1);
    open_board(screen, board, x - 1, y - 1);
}

int main(int argc, char** argv) {
    srand(time(NULL));
    
    renderer_init();

    char board[SCREEN_X][SCREEN_Y];
    char screen[SCREEN_X][SCREEN_Y];
    char number_of_mines = 10;
    char number_of_clear_fields = SCREEN_X * SCREEN_Y - number_of_mines;

    for (int x = 0; x < SCREEN_X; ++x) {
        for (int y = 0; y < SCREEN_Y; ++y) {
            board[x][y] = 0;
            screen[x][y] = COLOUR_UNKNOWN;
        }
    }

    for (int x = 0; x < SCREEN_X; ++x) {
        for (int y = 0; y < SCREEN_Y; ++y) {
            char random_cell = rand() % (number_of_clear_fields + number_of_mines) + 1;
            if (random_cell <= number_of_mines) {
	            board[x][y] = MINE;
                --number_of_mines;
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        if (x + i >= 0 && x + i < SCREEN_X && y + j >= 0 && y + j < SCREEN_Y && board[x+i][y+j] != MINE) {
                            ++board[x+i][y+j];
                        }
                    }
                }
            } else {
                --number_of_clear_fields;
            }
	    }
    }

    renderer_render((char *) screen);

    while (running) {
        enum Event event = EVENT_EMPTY;
        struct Position position;
        do {
            event = renderer_get_event();
            switch(event) {
                case EVENT_EXIT:
                    running = false;
                    break;
                case EVENT_BLOCK_TRY:
                    renderer_get_click(&position);
                    open_board(screen, board, position.x, position.y);
                    renderer_render((char *) screen);
                    break;
                case EVENT_BLOCK_MARK:
                    renderer_get_click(&position);
                    if (screen[position.x][position.y] == COLOUR_UNKNOWN) {
                        screen[position.x][position.y] = COLOUR_MARK;
                    } else if (screen[position.x][position.y] == COLOUR_MARK) {
                        screen[position.x][position.y] = COLOUR_UNKNOWN;
                    }
                    renderer_render((char *) screen);
                    break;
            }
        } while (event != EVENT_EMPTY);
    }
    
    renderer_destroy();

    return 0;
}
