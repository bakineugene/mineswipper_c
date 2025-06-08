#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define BLOCK_SIZE 35
#define SCREEN_WIDTH BLOCK_SIZE * SCREEN_X
#define SCREEN_HEIGHT BLOCK_SIZE * SCREEN_Y
#define WINDOW_TITLE "Mineswipper on SDL2"

#define SCREEN_X 9
#define SCREEN_Y 9

enum Block_Types {
    TYPE_EMPTY = 0,
    TYPE_UNKNOWN = 10,
    TYPE_MINE = 11,
    TYPE_MARK = 12,
};

/**
    SDL specific part
*/

/**
    Colour palette
*/

static const struct SDL_Color colour_red = { 196, 54, 56 };
static const struct SDL_Color colour_orange = { 230, 100, 20 };
static const struct SDL_Color colour_yellow = { 224, 204, 26 };
static const struct SDL_Color colour_green = { 24, 226, 112 };
static const struct SDL_Color colour_blue = { 26, 189, 224 };
static const struct SDL_Color colour_deep_blue = { 25, 90, 225 };
static const struct SDL_Color colour_violet = { 169, 27, 222 };
static const struct SDL_Color colour_black = { 0, 0, 0 };
static const struct SDL_Color colour_white = { 255, 255, 255 };
struct SDL_Color background_colour = colour_white;

/**
    Converting SDL events to in game events
*/

int click_x = 0;
int click_y = 0;

enum Event {
    EVENT_EMPTY,
    EVENT_EXIT,
    EVENT_BLOCK_TRY,
    EVENT_BLOCK_MARK
};

enum Event renderer_get_event() {
    static SDL_Event event;
    if (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                return EVENT_EXIT;

            case SDL_MOUSEBUTTONUP:
                click_x = event.button.x / BLOCK_SIZE;
                click_y = event.button.y / BLOCK_SIZE;
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

/**
    Textures for 1 - 9 numbers
*/
SDL_Texture* messages[9];

static SDL_Window* window;
static SDL_Renderer* renderer;

int renderer_init(void) {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return -1;
    }
    TTF_Init();
    TTF_Font* Sans = TTF_OpenFont("font.ttf", 100);

    window = SDL_CreateWindow(WINDOW_TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!window) {
        printf("Could not create a window: %s\n", SDL_GetError());
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!renderer) {
        printf("Could not create a renderer: %s\n", SDL_GetError());
        return -1;
    }

    messages[0] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "1", colour_white));
    messages[1] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "2", colour_white));
    messages[2] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "3", colour_white));
    messages[3] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "4", colour_white));
    messages[4] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "5", colour_white));
    messages[5] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "6", colour_white));
    messages[6] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "7", colour_white));
    messages[7] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "8", colour_white));
    messages[8] = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Solid(Sans, "9", colour_white));

    SDL_SetRenderDrawColor(renderer, background_colour.r, background_colour.g, background_colour.b, 255);
}

void renderer_render(char *a) {
    SDL_RenderClear(renderer);

	for (int x = 0; x < SCREEN_X; ++x) {
	    for (int y = 0; y < SCREEN_Y; ++y) {
            struct SDL_Color colour;
            SDL_Texture* message;
            char s = *(a + x * SCREEN_Y + y);
            switch(s) {
                case TYPE_MARK:
                    colour = colour_blue;
                    message = NULL;
                    break;
                case TYPE_MINE:
                    colour = colour_red;
                    message = NULL;
                    break;
                case TYPE_UNKNOWN:
                    colour = colour_white;
                    message = NULL;
                    break;
                case TYPE_EMPTY:
                    colour = colour_black;
                    message = NULL;
                    break;
                default:
                    colour = colour_black;
                    message = messages[s - 1];
                    break;
            }

            SDL_Rect rect;
            rect.x = 0 + x * BLOCK_SIZE;
            rect.y = 0 + y * BLOCK_SIZE;
            rect.w = BLOCK_SIZE;
            rect.h = BLOCK_SIZE;

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
            SDL_RenderDrawRect(renderer, &rect);
            rect.x += 2;
            rect.y += 2;
            rect.w -= 4;
            rect.h -= 4;
            SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, 80);
            SDL_RenderFillRect(renderer, &rect);

            if (message != NULL) {
                rect.x += 4;
                rect.y += 4;
                rect.w -= 8;
                rect.h -= 8;
                SDL_RenderCopy(renderer, message, NULL, &rect);
            }
	    }
	}
    SDL_SetRenderDrawColor(renderer, background_colour.r, background_colour.g, background_colour.b, 255);

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

void open_board(char screen[SCREEN_X][SCREEN_Y], char board[SCREEN_X][SCREEN_Y], char x, char y) {
    if (x < 0 || x >= SCREEN_X || y < 0 || y >= SCREEN_Y) return;

    if (board[x][y] > 0 && board[x][y] <= 9) {
        screen[x][y] = board[x][y];
        return;
    }
    if (board[x][y] == TYPE_MINE) {
        memcpy(screen, board, SCREEN_X * SCREEN_Y * sizeof(char));
        return;
    }
    if (board[x][y] == screen[x][y]) {
        return;
    }

    screen[x][y] = board[x][y];

    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            open_board(screen, board, x + i, y + j);
        }
    }
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
            screen[x][y] = TYPE_UNKNOWN;
        }
    }

    for (int x = 0; x < SCREEN_X; ++x) {
        for (int y = 0; y < SCREEN_Y; ++y) {
            char random_cell = rand() % (number_of_clear_fields + number_of_mines) + 1;
            if (random_cell <= number_of_mines) {
	            board[x][y] = TYPE_MINE;
                --number_of_mines;
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        if (x + i >= 0 && x + i < SCREEN_X && y + j >= 0 && y + j < SCREEN_Y && board[x+i][y+j] != TYPE_MINE) {
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
        do {
            event = renderer_get_event();
            switch(event) {
                case EVENT_EXIT:
                    running = false;
                    break;
                case EVENT_BLOCK_TRY:
                    open_board(screen, board, click_x, click_y);
                    renderer_render((char *) screen);
                    break;
                case EVENT_BLOCK_MARK:
                    if (screen[click_x][click_y] == TYPE_UNKNOWN) {
                        screen[click_x][click_y] = TYPE_MARK;
                    } else if (screen[click_x][click_y] == TYPE_MARK) {
                        screen[click_x][click_y] = TYPE_UNKNOWN;
                    }
                    renderer_render((char *) screen);
                    break;
            }
        } while (event != EVENT_EMPTY);
    }
    
    renderer_destroy();

    return 0;
}
