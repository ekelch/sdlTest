#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_keycode.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_scancode.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_timer.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

const int SCREEN_WIDTH = 1100;
const int SCREEN_HEIGHT = 700;

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

class LTimer {
    public:
        LTimer();
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
SDL_Event e;

LTexture gBackground;
LTexture gSprites;
LTexture gFontTexture;

SDL_Rect gSpriteClips[4];
SDL_Rect* currentSpriteClip = NULL;
//anime
const int ANIME_NUM_FRAMES = 4;
LTexture gAnimeSprite;
SDL_Rect gAnimeClips[ANIME_NUM_FRAMES];
int frame = 0;
int frameBuffer = 0;
//anime

Uint64 starttime = 0;
std::stringstream timeText;
TTF_Font* gFont;
SDL_Color fontColor = {0,0,0,255};
LTimer lTimer;

Mix_Music* gMusic = NULL;

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
            if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_SPACE:
                        if (Mix_PlayingMusic() == 0) {
                            Mix_PlayMusic(gMusic, 0);
                        } else {
                            if (Mix_PausedMusic() == 1) {
                                Mix_ResumeMusic();
                            } else {
                                Mix_PauseMusic();
                            }
                        }
                        break;
                    case SDLK_s:
                        lTimer.toggleStart();
                        break;
                    case SDLK_p:
                        lTimer.togglePause();
                        break;
                }
            }

        }

        const Uint8* keyStates = SDL_GetKeyboardState(NULL);
        if (keyStates[SDL_SCANCODE_UP]) {
            currentSpriteClip = &gSpriteClips[0];
        } else if (keyStates[SDL_SCANCODE_DOWN]) {
            currentSpriteClip = &gSpriteClips[1];
        } else if (keyStates[SDL_SCANCODE_LEFT]) {
            currentSpriteClip = &gSpriteClips[2];
        } else if (keyStates[SDL_SCANCODE_RIGHT]) {
            currentSpriteClip = &gSpriteClips[3];
        }

        SDL_SetRenderDrawColor(gRenderer, 1, 1, 1, 1);
        SDL_RenderClear(gRenderer);

        gBackground.render(0, 0, NULL);
        SDL_Rect *currentClip = &gAnimeClips[frame / 4];
        gAnimeSprite.render(120, 195, currentClip);

        if (currentSpriteClip == NULL) {
            gSprites.render(0, SCREEN_HEIGHT - 200);
        } else {
            gSprites.render(0, SCREEN_HEIGHT - 100, currentSpriteClip);
        }

        timeText.str("");
        timeText << lTimer.getTicks() / 1000.0f;
        if (!gFontTexture.loadFromRenderedText(timeText.str().c_str(), fontColor)) {
            printf("Failed to load text");
        }
        gFontTexture.render(400, 120);

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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        return false;
    }
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
    TTF_Init();
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("SDL mixer could not be initialized!\nMIXERROR: %s\n", Mix_GetError());
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

    gFont = TTF_OpenFont("resources/lazy.ttf", 20);
    if (gFont == NULL) {
        printf("Failed to load lazy text!\n%s\n", TTF_GetError());
        return false;
    }
    if (!gFontTexture.loadFromRenderedText("I'm so fucking gay", fontColor)) {
        printf("Failed load font texture\n%s\n", TTF_GetError());
        return false;
    }

    if (!gBackground.loadTexture("resources/background.png")) {
        printf("Failed to load background!\n%s\n", IMG_GetError());
        return false;
    }
    if (!gSprites.loadTexture("resources/sprites.png")) {
        printf("Failed to load sprite!\n%s\n", IMG_GetError());
        return false;
    }
    for (int i = 0; i < 4; i++) {
        gSpriteClips[i].x = i % 2 * 100;
        gSpriteClips[i].y = i / 2 * 100;
        gSpriteClips[i].w = 100;
        gSpriteClips[i].h = 100;
    }

    gMusic = Mix_LoadMUS("resources/mice.mp3");
    if (gMusic == NULL) {
        printf("Failed to load mice!\n%s\n", Mix_GetError());
        return false;
    }

    return true;
}

void close() {
    gAnimeSprite.freeTexture();
    gBackground.freeTexture();

    SDL_DestroyRenderer(gRenderer);
    gRenderer = NULL;
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    Mix_FreeMusic(gMusic);
    gMusic = NULL;

    Mix_Quit();
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
