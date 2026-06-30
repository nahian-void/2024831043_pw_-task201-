#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <SDL_ttf.h>

#define windowwidth  600
#define windowheight 600
#define cellsize     20
#define ROWS (windowheight / cellsize)
#define COLS (windowwidth / cellsize)
#define maxsnake 400

SDL_Window   *window   = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font     *font     = NULL;

int snakeX[maxsnake];
int snakeY[maxsnake];
int snakeLength;
int foodX;
int foodY;
int moveX = 1;
int moveY = 0;

bool running  = true;
bool gameOver = false;

int score = 0;

void spawnFood()
{
    bool ok = false;
    while (!ok)
    {
        ok = true;
        foodX = rand() % COLS;
        foodY = rand() % ROWS;

        for (int i = 0; i < snakeLength; i++)
        {
            if (snakeX[i] == foodX && snakeY[i] == foodY)
            {
                ok = false;
                break;
            }
        }
    }
}

void makesnake()
{
    snakeLength = 3;
    snakeX[0] = 10;
    snakeY[0] = 10;
    snakeX[1] = 9;
    snakeY[1] = 10;
    snakeX[2] = 8;
    snakeY[2] = 10;
}

void drawcell(int x, int y)
{
    SDL_Rect rect;
    rect.x = x * cellsize;
    rect.y = y * cellsize;
    rect.w = cellsize;
    rect.h = cellsize;

    SDL_RenderFillRect(renderer, &rect);
}

void drawSnake()
{
    for (int i = 0; i < snakeLength; i++)
    {
        if (i == 0)
        {
            SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
        }
        else
        {
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        }
        drawcell(snakeX[i], snakeY[i]);
    }
}

void drawFood()
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    drawcell(foodX, foodY);
}

void eventhandle()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            running = false;

        if (event.type == SDL_KEYDOWN)
        {
            switch (event.key.keysym.sym)
            {
                case SDLK_UP:
                    if (moveY != 1)
                    {
                        moveX = 0;
                        moveY = -1;
                    }
                    break;

                case SDLK_DOWN:
                    if (moveY != -1)
                    {
                        moveX = 0;
                        moveY = 1;
                    }
                    break;

                case SDLK_LEFT:
                    if (moveX != 1)
                    {
                        moveX = -1;
                        moveY = 0;
                    }
                    break;

                case SDLK_RIGHT:
                    if (moveX != -1)
                    {
                        moveX = 1;
                        moveY = 0;
                    }
                    break;
            }
        }
    }
}

void moveSnake()
{
    for (int i = snakeLength - 1; i > 0; i--)
    {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }
    snakeX[0] += moveX;
    snakeY[0] += moveY;
}

void checkFood()
{
    if (snakeX[0] == foodX && snakeY[0] == foodY)
    {
        snakeLength++;
        score = score + 10;
        //printf("Score : %d\n",score);
        spawnFood();
    }
}

void drawText(char text[], int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void checkWall()
{
    if (snakeX[0] < 0)    gameOver = true;
    if (snakeX[0] >= COLS) gameOver = true;
    if (snakeY[0] < 0)    gameOver = true;
    if (snakeY[0] >= ROWS) gameOver = true;
}

void checkSelf()
{
    for (int i = 1; i < snakeLength; i++)
    {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
        {
            gameOver = true;
            break;
        }
    }
}

void render()
{
    if (gameOver)
    {
        SDL_SetRenderDrawColor(renderer, 230, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_Color white = {255, 255, 255, 255};
        char scoreText[50];
        sprintf(scoreText, "Final Score : %d", score);

        drawText("GAME OVER", 170, 220, white);
        drawText(scoreText, 170, 280, white);

        SDL_RenderPresent(renderer);
        return;
    }

    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    drawSnake();
    drawFood();

    SDL_Color white = {255, 255, 255, 255};
    char scoreText[50];
    sprintf(scoreText, "Score : %d", score);
    drawText(scoreText, 10, 10, white);

    SDL_RenderPresent(renderer);
}

int main(int argc, char *argv[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 36);
    if (font == NULL)
    {
        printf("Font loading failed\n");
        return 1;
    }

    window = SDL_CreateWindow("Snake Game",
                               SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               windowwidth, windowheight, 0);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    makesnake();
    spawnFood();

    while (running)
    {
        eventhandle();

        if (!gameOver)
        {
            moveSnake();
            checkFood();
            checkWall();
            checkSelf();
        }
        else
        {
            SDL_SetWindowTitle(window, "GAME OVER");
        }

        render();
        SDL_Delay(120);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 0;
}