#include <SDL2/SDL.h>
#include <stdbool.h>

const int width = 800;
const int height = 600;
const int snake_block = 10;
const int snake_speed = 15;

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    bool running = true;
    SDL_Event event;

    int x1 = width / 2;
    int y1 = height / 2;
    int x1_change = 0;
    int y1_change = 0;

    Uint32 start_time;
    int frame_delay = 1000 / snake_speed;

    while (running) {
        start_time = SDL_GetTicks();

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        x1_change = -snake_block;
                        y1_change = 0;
                        break;
                    case SDLK_RIGHT:
                        x1_change = snake_block;
                        y1_change = 0;
                        break;
                    case SDLK_UP:
                        x1_change = 0;
                        y1_change = -snake_block;
                        break;
                    case SDLK_DOWN:
                        x1_change = 0;
                        y1_change = snake_block;
                        break;
                }
            }
        }

        x1 += x1_change;
        y1 += y1_change;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect snake_rect = {x1, y1, snake_block, snake_block};
        SDL_RenderFillRect(renderer, &snake_rect);

        SDL_RenderPresent(renderer);

        if (1000 / snake_speed > SDL_GetTicks() - start_time) {
            SDL_Delay(frame_delay - (SDL_GetTicks() - start_time));
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
