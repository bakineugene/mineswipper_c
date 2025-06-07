#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "events.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

/* macros */
#define BRICK_SIZE 50
#define SCREEN_WIDTH BRICK_SIZE * SCREEN_X
#define SCREEN_HEIGHT BRICK_SIZE * SCREEN_Y
#define WINDOW_TITLE "Mineswipper on SDL2"

struct Position {
    char x;
    char y;
};

/* variables */
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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                            return EVENT_LEFT;
                    case SDLK_RIGHT:
                            return EVENT_RIGHT;
                    case SDLK_DOWN:
                            return EVENT_DOWN;
                    case SDLK_UP:
                            return EVENT_UP;
                    case SDLK_SPACE:
                            return EVENT_SPACE;
                }

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
