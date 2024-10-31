#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// Window rendered to
SDL_Window* gWindow = NULL;
// Window Renderer
SDL_Renderer* gRenderer = NULL;
// Current displayed texture
SDL_Texture* gTexture = NULL;
SDL_Event e;

// Loads individual image as texture
SDL_Texture* loadTexture(char *path);

bool init();
bool loadMedia();
void close();

int main(int argc, char *argv[]) {
    bool quit = false;

    if (!init()) {
        printf("Failed to init");
        return 0;
    }
    if (!loadMedia()) {
        printf("Failed to load media");
        return 0;
    }
    while (!quit) {
        // 0 -> nothing in queue
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        SDL_RenderClear(gRenderer);
        SDL_RenderCopy(gRenderer, gTexture, 0, 0);
        SDL_RenderPresent(gRenderer);
    }

    return 0;
}

bool init() {
    gWindow = SDL_CreateWindow("Hello World :3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Failed to create window!\nSDL_Error: %s\n", SDL_GetError());
        return false;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
    if (gRenderer == NULL) {
        printf("Failed to create renderer!\nSDL_Error: %s\n", SDL_GetError());
        return false;
    }
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) && imgFlags)) {
        printf("SDL Image could not be initialized!\nSDL_image Error: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

SDL_Texture* loadTexture(char *path) {
    // new texture to be returned
    SDL_Texture* newTexture = NULL;

    // Load image at path
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Failed to load surface from %s!\nSDL_image Error: %s\n", path, IMG_GetError());
        return NULL;
    }

    // Creating texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
        printf("Unable to create texture from %s!\nSDL_image Error: %s\n", path, IMG_GetError());
    }

    SDL_FreeSurface(loadedSurface);
    return newTexture;
}

bool loadMedia() {
    gTexture = loadTexture("resources/gGradient.png");
    if (gTexture == NULL) {
        printf("Failed to load media!\n");
        return false;
    }
    return true;
}

void close() {
    SDL_DestroyTexture(gTexture);
    gTexture = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;

    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}
