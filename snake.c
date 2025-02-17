#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

const int width = 800;
const int height = 600;
const int snake_block = 10;
const int base_snake_speed = 15;

typedef struct {
    int x, y;
} SnakeSegment;

void drawSnake(SDL_Renderer* renderer, SnakeSegment* snake, int length) {
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < length; i++) {
        SDL_Rect snake_rect = {snake[i].x, snake[i].y, snake_block, snake_block};
        SDL_RenderFillRect(renderer, &snake_rect);
    }
}

bool checkCollision(SnakeSegment* snake, int length) {
    for (int i = 1; i < length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return true;
        }
    }
    return false;
}

bool checkFoodCollision(SnakeSegment* snake, int food_x, int food_y) {
    if (snake[0].x == food_x && snake[0].y == food_y) {
        return true;
    }
    return false;
}

bool checkBoundaryCollision(SnakeSegment* snake) {
    if (snake[0].x < 0 || snake[0].x >= width || snake[0].y < 0 || snake[0].y >= height) {
        return true;
    }
    return false;
}

void generateFood(int* food_x, int* food_y) {
    *food_x = (rand() % (width / snake_block)) * snake_block;
    *food_y = (rand() % (height / snake_block)) * snake_block;
}

void renderText(SDL_Renderer* renderer, TTF_Font* font, const char* text, int x, int y) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, white);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void updateScoreboard(int score) {
    FILE* scoreboard_file = fopen("scoreboard.txt", "a");
    if (scoreboard_file != NULL) {
        fprintf(scoreboard_file, "%d\n", score);
        fclose(scoreboard_file);
    }
}

void displayScoreboard(SDL_Renderer* renderer, TTF_Font* font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    renderText(renderer, font, "High Scores:", width / 2 - 50, 50);

    FILE* scoreboard_file = fopen("scoreboard.txt", "r");
    if (scoreboard_file != NULL) {
        char line[256];
        int y = 100;
        while (fgets(line, sizeof(line), scoreboard_file)) {
            int score;
            sscanf(line, "%d", &score);
            char score_text[50];
            sprintf(score_text, "%d", score);
            renderText(renderer, font, score_text, width / 2 - 20, y);
            y += 30;
        }
        fclose(scoreboard_file);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(5000); // Display scoreboard for 5 seconds
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font* font = TTF_OpenFont("arial.ttf", 24);

    srand(time(NULL));

    bool running = true;
    bool paused = false;
    SDL_Event event;

    int x1 = width / 2;
    int y1 = height / 2;
    int x1_change = 0;
    int y1_change = 0;

    int food_x, food_y;
    generateFood(&food_x, &food_y);

    Uint32 start_time;
    int frame_delay = 1000 / base_snake_speed;
    int snake_speed = base_snake_speed;

    SnakeSegment snake[100];
    int snake_length = 1;
    snake[0].x = x1;
    snake[0].y = y1;

    int score = 0;

    while (running) {
        start_time = SDL_GetTicks();

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_p) {
                    paused = !paused;
                }
                if (!paused) {
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
        }

        if (!paused) {
            x1 += x1_change;
            y1 += y1_change;

            for (int i = snake_length - 1; i > 0; i--) {
                snake[i] = snake[i - 1];
            }
            snake[0].x = x1;
            snake[0].y = y1;

            if (checkCollision(snake, snake_length) || checkBoundaryCollision(snake)) {
                updateScoreboard(score);
                running = false;
            }

            if (checkFoodCollision(snake, food_x, food_y)) {
                snake_length++;
                score++;
                snake_speed = base_snake_speed + score / 5; // Increase speed every 5 points
                generateFood(&food_x, &food_y);
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_Rect food_rect = {food_x, food_y, snake_block, snake_block};
            SDL_RenderFillRect(renderer, &food_rect);

            drawSnake(renderer, snake, snake_length);

            char score_text[50];
            sprintf(score_text, "Score: %d", score);
            renderText(renderer, font, score_text, 10, 10);

            SDL_RenderPresent(renderer);

            if (1000 / snake_speed > SDL_GetTicks() - start_time) {
                SDL_Delay(frame_delay - (SDL_GetTicks() - start_time));
            }
        }
    }

    displayScoreboard(renderer, font);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
