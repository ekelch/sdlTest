#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <wchar.h>

#define WIDTH 1600
#define HEIGHT 900

SDL_Window* gWindow = NULL;
SDL_Surface* gScreenSurface = NULL;
SDL_Surface* gHelloWorld = NULL;

bool init();
bool loadMedia();
void close();

int main(int argc, char* argv[]) {

    if (!init()) {
        printf("Failed to initialize!");
    } else {
        if (!loadMedia()) {
            printf("Failed to load media!");
        } else {
            SDL_BlitSurface(gHelloWorld, NULL, gScreenSurface, NULL);
            SDL_UpdateWindowSurface(gWindow);

            SDL_Event e;
            bool quit = false;
            while(quit == false) {
                while(SDL_PollEvent(&e)) {
                    if(e.type == SDL_QUIT)
                        quit = true;
                }
            }
        }
    }

    close();
    return 0;
}

bool init() {
    //init flag
    bool success = true;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to init SDL, SDL_Error: %s\n", SDL_GetError());
        success = false;
    } else {
        //create window
        gWindow = SDL_CreateWindow(
        "SDL Test",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WIDTH,
        HEIGHT,
        0
        );

        if (gWindow == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        } else {
            gScreenSurface = SDL_GetWindowSurface(gWindow);
        }
    }
    return success;
}

bool loadMedia() {
    bool success = true;

    gHelloWorld = SDL_LoadBMP("sample.bmp");
    if (gHelloWorld == NULL) {
        printf("Media could not be loaded! SDL_Error: %s\n", SDL_GetError());

        success = false;
    }

    return success;
}

void close() {
    //Deallocate surface
    SDL_FreeSurface(gHelloWorld);
    gHelloWorld = NULL;

    //Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //quit sdl subsystems
    SDL_Quit();
}