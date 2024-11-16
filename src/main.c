#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_image.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_video.h"
#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>
#include <stdlib.h>
#include <string.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int MAX_RESOURCE_PATH = 40;

typedef struct LTexture{
    SDL_Texture* texture;
    int w;
    int h;

    bool (*loadTexture)(char* path, struct LTexture *this);
    void (*render)(int x, int y, struct LTexture *this);
    void (*free)(struct LTexture *this);
} LTexture;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Event e;

bool loadTexture(char *path, LTexture* lTexture);
void renderTexture(int x, int y, struct LTexture *l);
void freeTexture(struct LTexture *l);
LTexture* newTexture();

LTexture fooTexture;
LTexture backTexture;

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

        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
        SDL_RenderPresent(gRenderer);

        backTexture.render(0, 0, &backTexture);
        fooTexture.render(100, 100, &fooTexture);

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

// load texture into gTexture
bool loadMedia() {
    fooTexture = *newTexture();
    if (!fooTexture.loadTexture("resources/foo.png", &fooTexture)) {
        printf("Failed to load foo!\n");
        return false;
    }

    backTexture = *newTexture();
    if (!backTexture.loadTexture("resources/background.png", &backTexture)) {
        printf("Failed to load background!\n");
        return false;
    }
    return true;
}

void close() {
    fooTexture.free(&fooTexture);
    backTexture.free(&backTexture);

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}

//LTexture

LTexture* newTexture() {
    LTexture *lt = malloc(sizeof(LTexture));
    lt->texture = NULL;
    lt->w = 0;
    lt->h = 0;
    lt->loadTexture = loadTexture;
    lt->render = renderTexture;
    lt->free = freeTexture;
    return lt;
}


// Render texture at x,y coords
bool loadTexture(char *path, LTexture *this) {

    // Load image at path
    SDL_Surface* loadedSurface = IMG_Load(path);
    if (loadedSurface == NULL) {
        printf("Failed to load surface from %s!\nSDL_image Error: %s\n", path, IMG_GetError());
        return false;
    }

    // surface to key, enable keying, pixel to key with (cyan//mapRGB is cross platform compatible), loaded surface has format var
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface -> format, 0, 255, 255));

    // Creating texture from surface pixels
    this->texture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (this->texture == NULL) {
        printf("Unable to create texture from %s!\nSDL_image Error: %s\n", path, IMG_GetError());
        return false;
    }

    SDL_FreeSurface(loadedSurface);
    return true;
}

void renderTexture(int x, int y, struct LTexture *this) {
    SDL_Rect renderQuad = {x, y, this->w, this->h};
    SDL_RenderCopy(gRenderer, this->texture, NULL, &renderQuad);
}

void freeTexture(struct LTexture *this) {
    if (this->texture != NULL) {
        SDL_DestroyTexture(this->texture);
        this->texture = NULL;
        this->w = 0;
        this->h = 0;
    }
}
//LTexture
