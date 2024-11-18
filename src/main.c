#include "SDL_blendmode.h"
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
    void (*setColor)(Uint8 r, Uint8 g, Uint8 b, struct LTexture *this);
    void (*setBlendMode)(SDL_BlendMode mode, struct LTexture *this);
    void (*setAlpha)(Uint8 a, struct LTexture* this);
    void (*render)(int x, int y, SDL_Rect* clip, struct LTexture *this);
    void (*free)(struct LTexture *this);
} LTexture;

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Event e;
LTexture* newTexture();

LTexture gBackground;
//anime
const int ANIME_NUM_FRAMES = 4;
LTexture gAnimeSprite;
SDL_Rect gAnimeClips[ANIME_NUM_FRAMES];
int frame = 0;
int frameBuffer = 0;
//anime

//mod components
Uint8 r = 255;
Uint8 g = 255;
Uint8 b = 255;
Uint8 a = 255;

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
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_q:
                        r += 32;
                        break;
                    case SDLK_w:
                        g += 32;
                        break;
                    case SDLK_e:
                        b += 32;
                        break;
                    case SDLK_a:
                        r -= 32;
                        break;
                    case SDLK_s:
                        g -= 32;
                        break;
                    case SDLK_d:
                        b -= 32;
                        break;
                    case SDLK_r:
                        if (a + 10 > 255) {
                            a = 255;
                        } else {
                            a += 10;
                        }
                        break;
                    case SDLK_f:
                        if (a - 10 < 0) {
                            a = 0;
                        } else {
                            a -= 10;
                        }
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(gRenderer, 1, 1, 1, 1);
        SDL_RenderClear(gRenderer);

        gBackground.setColor(r, g, b, &gBackground);
        gBackground.render(0, 0, NULL, &gBackground);

        SDL_Rect *currentClip = &gAnimeClips[frame / 4];
        gAnimeSprite.render(120, 195, currentClip, &gAnimeSprite);

        SDL_RenderPresent(gRenderer);

        //Go to next frame
        if (++frameBuffer % 3 == 0) {
            frameBuffer = 0;
            ++frame;
        }

        //Cycle animation
        if(frame / ANIME_NUM_FRAMES >= ANIME_NUM_FRAMES) {
            frame = 0;
        }
    }

    return 0;
}

bool init() {
    gWindow = SDL_CreateWindow("Hello World :3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Failed to create window!\nSDL_Error: %s\n", SDL_GetError());
        return false;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED |  SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL) {
        printf("Failed to create renderer!\nSDL_Error: %s\n", SDL_GetError());
        return false;
    }
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) && imgFlags)) {
        printf("SDL Image could not be initialized!\nSDL_image Error: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

// load texture into gTexture
bool loadMedia() {
    gAnimeSprite = *newTexture();
    if (!gAnimeSprite.loadTexture("resources/anime.png", &gAnimeSprite)) {
        printf("Failed to load foo!\n%s\n", IMG_GetError());
        return false;
    }
    for (int i = 0; i < ANIME_NUM_FRAMES; i++) {
        gAnimeClips[i].x = 64 * i;
        gAnimeClips[i].y = 0;
        gAnimeClips[i].w = 64 ;
        gAnimeClips[i].h = 205;
    }

    gBackground = *newTexture();
    if (!gBackground.loadTexture("resources/background.png", &gBackground)) {
        printf("Failed to load background!\n%s\n", IMG_GetError());
        return false;
    }

    gAnimeSprite.setBlendMode(SDL_BLENDMODE_BLEND, &gAnimeSprite);

    return true;
}

void close() {
    gAnimeSprite.free(&gAnimeSprite);
    gBackground.free(&gBackground);

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    IMG_Quit();
    SDL_Quit();
}

//LTexture
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
    this->w = loadedSurface->w;
    this->h = loadedSurface->h;
    SDL_FreeSurface(loadedSurface);
    return true;
}

void renderTexture(int x, int y, SDL_Rect* clip, struct LTexture *this) {
    SDL_Rect renderQuad = {x, y, this->w, this->h};
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopy(gRenderer, this->texture, clip, &renderQuad);
}

void freeTexture(struct LTexture *this) {
    if (this->texture != NULL) {
        SDL_DestroyTexture(this->texture);
        this->texture = NULL;
        this->w = 0;
        this->h = 0;
    }
}

void setColor(Uint8 r, Uint8 g, Uint8 b, struct LTexture *this) {
    SDL_SetTextureColorMod(this->texture, r, g, b);
}

void setBlendMode(SDL_BlendMode blendMode, LTexture *this) {
    SDL_SetTextureBlendMode(this->texture, blendMode);
}

void setAlpha(Uint8 a, LTexture *this) {
    SDL_SetTextureAlphaMod(this->texture, a);
}

LTexture* newTexture() {
    LTexture *lt = malloc(sizeof(LTexture));
    lt->texture = NULL;
    lt->w = 0;
    lt->h = 0;
    lt->loadTexture = loadTexture;
    lt->setColor = setColor;
    lt->setBlendMode = setBlendMode;
    lt->setAlpha = setAlpha;
    lt->render = renderTexture;
    lt->free = freeTexture;
    return lt;
}
//LTexture
