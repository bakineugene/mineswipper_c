#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define BRICK_SIZE 50
#define SCREEN_WIDTH BRICK_SIZE * SCREEN_X
#define SCREEN_HEIGHT BRICK_SIZE * SCREEN_Y
#define WINDOW_TITLE "Mineswipper on SDL2"

#define SCREEN_X 9
#define SCREEN_Y 9

enum Colour {
    COLOUR_UNKNOWN = 10,
    COLOUR_MINE = 11,
    COLOUR_MARK = 12,
};


enum Event {
    EVENT_EMPTY,
    EVENT_EXIT,
    EVENT_BLOCK_TRY,
    EVENT_BLOCK_MARK
};

struct Position {
    char x;
    char y;
};

static SDL_Window* window;
static SDL_Renderer* renderer;
static SDL_Event event;

struct SDLColour {
    char red;
    char green;
    char blue;
};

static const struct SDLColour renderer_colour_red = { 196, 54, 56 };
static const struct SDLColour renderer_colour_orange = { 230, 100, 20 };
static const struct SDLColour renderer_colour_yellow = { 224, 204, 26 };
static const struct SDLColour renderer_colour_green = { 24, 226, 112 };
static const struct SDLColour renderer_colour_blue = { 26, 189, 224 };
static const struct SDLColour renderer_colour_deep_blue = { 25, 90, 225 };
static const struct SDLColour renderer_colour_violet = { 169, 27, 222 };
static const struct SDLColour renderer_colour_black = { 0, 0, 0 };
static const struct SDLColour renderer_colour_white = { 255, 255, 255 };


int click_x = 0;
int click_y = 0;

enum Event renderer_get_event() {
    if (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                return EVENT_EXIT;

            case SDL_MOUSEBUTTONUP:
                click_x = event.button.x / BRICK_SIZE;
                click_y = event.button.y / BRICK_SIZE;
                if (event.button.button == SDL_BUTTON_LEFT) {
                    return EVENT_BLOCK_TRY;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    return EVENT_BLOCK_MARK;
                }
                
        }
    }
    return EVENT_EMPTY;
}

void renderer_get_click(struct Position *position) {
    position->x = click_x;
    position->y = click_y;
}

struct SDLColour background_colour = renderer_colour_white;

int renderer_init(void) {
    /* Initializing SDL2 */
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }
    TTF_Init();
    /* Creating a SDL window */
    window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Could not create a window: %s\n", SDL_GetError());
        return -1;
    }

    /* Creating a renderer */
    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Could not create a renderer: %s\n", SDL_GetError());
        return -1;
    }

    SDL_SetRenderDrawColor(renderer, background_colour.red, background_colour.green, background_colour.blue, 255);
}

void renderer_render(char *a) {
    SDL_RenderClear(renderer);

	for (int x = 0; x < SCREEN_X; ++x) {
	    for (int y = 0; y < SCREEN_Y; ++y) {
            struct SDLColour colour;
            SDL_Texture* Message;
            bool has_colour = true;
            char s = *(a + x * SCREEN_Y + y);
            switch(s) {
                case 12:
                    colour = renderer_colour_blue;
                    Message = NULL;
                    break;
                case 11:
                    colour = renderer_colour_red;
                    Message = NULL;
                    break;
                case 10:
                    colour = renderer_colour_white;
                    Message = NULL;
                    break;
                case 0:
                    colour = renderer_colour_black;
                    Message = NULL;
                    break;
                default:
                    colour = renderer_colour_black;

                    TTF_Font* Sans = TTF_OpenFont("font4.ttf", 100);

                    SDL_Color White = {255, 255, 255};

                    char str[1];
                    sprintf(str, "%d", s);

                    SDL_Surface* surfaceMessage =
                        TTF_RenderText_Solid(Sans, str, White); 

                    Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
                    break;
            }
            if (has_colour) {
                SDL_Rect rect;
                rect.x = 0 + x * BRICK_SIZE;
                rect.y = 0 + y * BRICK_SIZE;
                rect.w = BRICK_SIZE;
                rect.h = BRICK_SIZE;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
                SDL_RenderDrawRect(renderer, &rect);
                rect.x += 2;
                rect.y += 2;
                rect.w -= 4;
                rect.h -= 4;
                SDL_SetRenderDrawColor(renderer, colour.red, colour.green, colour.blue, 80);
                SDL_RenderFillRect(renderer, &rect);

                if (Message != NULL) {
                    rect.x += 4;
                    rect.y += 4;
                    rect.w -= 8;
                    rect.h -= 8;
                    SDL_RenderCopy(renderer, Message, NULL, &rect);
                }
            }
	    }
	}
    /**
    SDL_SetRenderDrawColor(renderer, 111, 105, 145, 255);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for (int x = 0; x < 1 + SCREEN_X * 8 * 5; x += 8 * 5) {
        SDL_RenderDrawLine(renderer, x, 0, x, SCREEN_HEIGHT);
    }

	for (int y = 0; y < 1 + SCREEN_Y * 8 * 5; y += 8 * 5) {
        SDL_RenderDrawLine(renderer, 0, y, SCREEN_WIDTH, y);
    }
    */
    // SDL_SetRenderDrawColor(renderer, 110, 177, 255, 255);
    SDL_SetRenderDrawColor(renderer, background_colour.red, background_colour.green, background_colour.blue, 255);

    /* Showing what was drawn */
    SDL_RenderPresent(renderer);
}

void renderer_delay(int delay) {
    SDL_Delay(delay);
}

void renderer_destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
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
