#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_timer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cstdio>
#include <cstdlib>
#include <string>

const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 700;
const int MAX_FPS = 60;
const int TICKS_PER_FRAME = 1000 / MAX_FPS;

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

class Dot {
    public:
        static const int DOT_WIDTH = 20;
        static const int DOT_HEIGHT = 20;
        static const int DOT_VEL = 5;

        Dot();

        void handleEvent(SDL_Event& e);
        void move();
        void render();
    private:
        int mPosX, mPosY;
        int mVelX, mVelY;
};

class LTimer {
    public:
        LTimer();
        void start();
        void stop();
        void toggleStart();
        void togglePause();

        Uint64 getTicks();

        bool isStarted();
        bool isPaused();

    private:
        Uint64 mStartTick;
        Uint64 mPauseTick;
        bool mPaused;
        bool mStarted;
};

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont = NULL;

LTexture gSpriteTexture;

bool init();
bool loadMedia();
void close();

int main(int argc, char *argv[]) {
    SDL_Event e;
    bool quit = false;
    Dot dot;

    if (!init()) {
        printf("Failed to init");
        return 0;
    }
    if (!loadMedia()) {
        printf("Failed to load media");
        return 0;
    }
    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            dot.handleEvent(e);
        }
        dot.move();
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 1);
        SDL_RenderClear(gRenderer);
        dot.render();
        SDL_RenderPresent(gRenderer);
    }

    return 0;
}

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
    gWindow = SDL_CreateWindow("Hello World :3", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL) {
        printf("Failed to create window!\nSDL_Error: %s\n", SDL_GetError());
        return false;
    }
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
    if (!gSpriteTexture.loadTexture("resources/minecraft.png")) {
        printf("Failed to load sprite:\n%s\n", IMG_GetError());
        return false;
    }
    return true;
}

void close() {
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

#if defined(SDL_TTF_MAJOR_VERSION)
bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	//Render text surface
	SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	if( textSurface != NULL )
	{
		//Create texture from surface pixels
        mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );
		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		//Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}


	//Return success
	return mTexture != NULL;
}
#endif
//end LTexture

LTimer::LTimer() {
    mStartTick = 0;
    mPauseTick = 0;
    mStarted = false;
    mPaused = false;
}

void LTimer::start() {
    mStarted = true;
    mPaused = false;
    mStartTick = SDL_GetTicks64();
    mPauseTick = 0;
}

void LTimer::stop() {
    mStarted = false;
    mPaused = false;
    mStartTick = 0;
    mPauseTick = 0;
}

void LTimer::toggleStart() {
    if (mStarted) {
        mStarted = false;
        mPaused = false;
        mStartTick = 0;
        mPauseTick = 0;
    } else {
        mStarted = true;
        mPaused = false;
        mStartTick = SDL_GetTicks64();
        mPauseTick = 0;
    }
}

void LTimer::togglePause() {
    if (mStarted) {
        if (mPaused) {
            mPaused = false;
            mStartTick = SDL_GetTicks64() - mPauseTick;
        } else {
            mPaused = true;
            mPauseTick = SDL_GetTicks64() - mStartTick;
        }
    }
}

Uint64 LTimer::getTicks() {
    if (mStarted) {
        if (mPaused) {
            return mPauseTick;
        } else {
            return SDL_GetTicks() - mStartTick;
        }
    } else {
        return 0;
    }
}

bool LTimer::isStarted() {
    return mStarted;
}

bool LTimer::isPaused() {
    return mStarted && mPaused;
}

Dot::Dot() {
    mPosX = 0;
    mPosY = 0;
    mVelX = 0;
    mVelY = 0;
}

void Dot::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP: mVelY -= DOT_VEL; break;
            case SDLK_DOWN: mVelY += DOT_VEL; break;
            case SDLK_LEFT: mVelX -= DOT_VEL; break;
            case SDLK_RIGHT: mVelX += DOT_VEL; break;
        }
    }
    if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch (e.key.keysym.sym) {
            case SDLK_UP: mVelY += DOT_VEL; break;
            case SDLK_DOWN: mVelY -= DOT_VEL; break;
            case SDLK_LEFT: mVelX += DOT_VEL; break;
            case SDLK_RIGHT: mVelX -= DOT_VEL; break;
        }
    }
}

void Dot::move() {
    int updX = mPosX + mVelX;
    int updY = mPosY + mVelY;
    if (updX > 0 && updX < SCREEN_WIDTH) {
        mPosX = updX;
    }
    if (updY > 0 && updY < SCREEN_HEIGHT) {
        mPosY = updY;
    }
}

void Dot::render() {
    SDL_Rect grassClip = {16, 0, 16, 16};
    gSpriteTexture.render(mPosX, mPosY, &grassClip);
}
