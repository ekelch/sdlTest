#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_mouse.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_surface.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <cstdio>
#include <cstdlib>
#include <string>

const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 700;

const int BUTTON_HEIGHT = 200;
const int BUTTON_WIDTH = 300;
const int NUM_BUTTONS = 4;

enum LButtonSprite {
    BUTTON_SPRITE_MOUSE_OUT,
    BUTTON_SPRITE_MOUSE_OVER_MOTION,
    BUTTON_SPRITE_MOUSE_DOWN,
    BUTTON_SPRITE_MOUSE_UP,
    BUTTON_SPRITE_TOTAL
};

class LTexture{
    public:
        LTexture();
        ~LTexture();

        int mWidth;
        int mHeight;

        bool loadTexture(std::string path);
        void setColor(Uint8 r, Uint8 g, Uint8 b);
        void setBlendMode(SDL_BlendMode mode);
        void setAlpha(Uint8 a);
        void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
        void freeTexture();

        #if defined (SDL_TTF_MAJOR_VERSION)
        bool loadFromRenderedText(std::string text, SDL_Color color);
        #endif
    private:
        SDL_Texture* mTexture;
};

class LButton {
    public:
        LButton();
        void setPosition(int x, int y);
        void handleEvent(SDL_Event* e);
        void render();
    private:
        SDL_Point mPosition;
        LButtonSprite mCurrentSprite;
};

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;
SDL_Event e;

LTexture gBackground;
LTexture gFontTexture;
//anime
const int ANIME_NUM_FRAMES = 4;
LTexture gAnimeSprite;
SDL_Rect gAnimeClips[ANIME_NUM_FRAMES];
int frame = 0;
int frameBuffer = 0;
double angle = 0.0;
SDL_RendererFlip flipType;
//anime

//button
LTexture buttonTexture;
SDL_Rect buttonClips[NUM_BUTTONS];
LButton gButton;
//button

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
            }
            gButton.handleEvent(&e);

            if (e.type == SDL_KEYDOWN) {
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
                    case SDLK_z:
                        angle += 60;
                        break;
                    case SDLK_x:
                        angle -= 60;
                        break;
                    case SDLK_c:
                        flipType = SDL_FLIP_NONE;
                        break;
                    case SDLK_v:
                        flipType = SDL_FLIP_HORIZONTAL;
                        break;
                    case SDLK_b:
                        flipType = SDL_FLIP_VERTICAL;
                        break;
                }
            }
        }


        SDL_SetRenderDrawColor(gRenderer, 1, 1, 1, 1);
        SDL_RenderClear(gRenderer);

        gBackground.setColor(r, g, b);
        gBackground.render(0, 0, NULL);

        SDL_Rect *currentClip = &gAnimeClips[frame / 4];
        gAnimeSprite.setAlpha(a);
        gAnimeSprite.render(120, 195, currentClip, angle, NULL, flipType);
        gFontTexture.render(177, 150);

        gButton.render();

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
    if (TTF_Init() == -1) {
        printf("SDL TTF could not be initialized!\nTTF_ERROR: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

// load texture into gTexture
bool loadMedia() {
    if (!gAnimeSprite.loadTexture("resources/anime.png")) {
        printf("Failed to load foo!\n%s\n", IMG_GetError());
        return false;
    }
    gAnimeSprite.setBlendMode(SDL_BLENDMODE_BLEND);
    for (int i = 0; i < ANIME_NUM_FRAMES; i++) {
        gAnimeClips[i].x = 64 * i;
        gAnimeClips[i].y = 0;
        gAnimeClips[i].w = 64 ;
        gAnimeClips[i].h = 205;
    }

    if (!gBackground.loadTexture("resources/background.png")) {
        printf("Failed to load background!\n%s\n", IMG_GetError());
        return false;
    }

    if (!buttonTexture.loadTexture("resources/button.png")) {
        printf("Failed to load button texture\n%s\n", IMG_GetError());
        return false;
    }
    for (int i = 0; i < NUM_BUTTONS; i++) {
        buttonClips[i].x = 0;
        buttonClips[i].y = BUTTON_HEIGHT * i;
        buttonClips[i].w = BUTTON_WIDTH;
        buttonClips[i].h = BUTTON_HEIGHT;
    }
    gButton.setPosition(0, SCREEN_HEIGHT - BUTTON_HEIGHT);

    gFont = TTF_OpenFont("resources/lazy.ttf", 28);
    if (gFont == NULL) {
        printf("Failed to load lazy.ttf!\n%s\n", TTF_GetError());
        return false;
    }
    SDL_Color textColor = {244, 244, 244};
    if (!gFontTexture.loadFromRenderedText("The quick brown dog jumped over the lazy fox!", textColor)) {
        printf("Failed to render text!\n%s\n", TTF_GetError());
        return false;
    }

    return true;
}

void close() {
    gAnimeSprite.freeTexture();
    gBackground.freeTexture();
    TTF_CloseFont(gFont);
    gFont = NULL;

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

//LTexture
bool LTexture::loadTexture(std::string path) {
    // Load image at path
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (loadedSurface == NULL) {
        printf("Failed to load surface from %s!\nSDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }

    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface -> format, 0, 255, 255));

    // Creating texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (mTexture == NULL) {
        printf("Unable to create texture from %s!\nSDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }
    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;
    SDL_FreeSurface(loadedSurface);
    return true;
}

#if defined (SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText(std::string text, SDL_Color color) {
    free(mTexture);
    SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, text.c_str(), color);
    if (textSurface == NULL) {
        printf("Unable to create font surface!\nTTF_ERROR: %s\n", TTF_GetError());
        return false;
    }
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL) {
        printf("Unable to create texture from surface\nTTF_ERROR: %s\n", SDL_GetError());
        return false;
    }
    mWidth = textSurface->w;
    mHeight = textSurface->h;
    SDL_FreeSurface(textSurface);
    return true;
};
#endif

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

void LTexture::freeTexture() {
    if (mTexture != NULL) {
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
    free(mTexture);
}

void LTexture::setColor(Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetTextureColorMod(mTexture, r, g, b);
}

void LTexture::setBlendMode(SDL_BlendMode blendMode) {
    SDL_SetTextureBlendMode(mTexture, blendMode);
}

void LTexture::setAlpha(Uint8 a) {
    SDL_SetTextureAlphaMod(mTexture, a);
}

LTexture::LTexture() {
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture() {
    free(mTexture);
}
//end LTexture

//LButton

LButton::LButton() {
    mPosition.x = 0;
    mPosition.y = 0;
    mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
};

void LButton::setPosition(int x, int y) {
    mPosition.x = x;
    mPosition.y = y;
};

void LButton::handleEvent(SDL_Event* e) {
    if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
        int x,y;
        SDL_GetMouseState(&x, &y);
        bool inside = false;
        if (x > mPosition.x && x < mPosition.x + BUTTON_WIDTH && y > mPosition.y && y < mPosition.y + BUTTON_HEIGHT) {
            inside = true;
        }
        if (!inside) {
            mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
        } else {
            switch (e->type) {
                case SDL_MOUSEMOTION:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
                    break;
                case SDL_MOUSEBUTTONUP:
                    mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
                    break;
            }
        }
    }
};

void LButton::render() {
    buttonTexture.render(mPosition.x, mPosition.y, &buttonClips[mCurrentSprite]);
};

//end Lbutton
