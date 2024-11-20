#include <SDL.h>
#include <SDL_image.h>
#include <cstdlib>
#include <string>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

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
        void render(int x, int y, SDL_Rect* clip = NULL);
        void freeTexture();
    private:
        SDL_Texture* mTexture;
};

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
SDL_Event e;

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

        gBackground.setColor(r, g, b);
        gBackground.render(0, 0, NULL);

        SDL_Rect *currentClip = &gAnimeClips[frame / 4];
        gAnimeSprite.setAlpha(a);
        gAnimeSprite.render(120, 195, currentClip);

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
    if (!gAnimeSprite.loadTexture("resources/anime.png")) {
        printf("Failed to load foo!\n%s\n", IMG_GetError());
        return false;
    }
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

    gAnimeSprite.setBlendMode(SDL_BLENDMODE_BLEND);

    return true;
}

void close() {
    gAnimeSprite.freeTexture();
    gBackground.freeTexture();

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

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

    // surface to key, enable keying, pixel to key with (cyan//mapRGB is cross platform compatible), loaded surface has format var
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

void LTexture::render(int x, int y, SDL_Rect* clip) {
    SDL_Rect renderQuad = {x, y, mWidth, mHeight};
    if (clip != NULL) {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
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
//LTexture
