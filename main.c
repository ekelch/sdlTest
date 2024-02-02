#include <stdio.h>
#include <SDL.h>
#include <stdbool.h>
#include <wchar.h>

#define WIDTH 1600
#define HEIGHT 900

enum KeyPressSurface {
    KEY_PRESS_SURFACE_DEFAULT,
    KEY_PRESS_SURFACE_UP,
    KEY_PRESS_SURFACE_DOWN,
    KEY_PRESS_SURFACE_LEFT,
    KEY_PRESS_SURFACE_RIGHT,
    KEY_PRESS_SURFACE_TOTAL,
};

SDL_Window *gWindow = NULL;
SDL_Surface *gScreenSurface = NULL;

SDL_Surface *gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];
SDL_Surface *gCurrentSurface = NULL;

bool init();

bool loadMedia();

void close();

SDL_Surface *loadSurface(char *path);

int main(int argc, char *argv[]) {
    if (!init()) {
        printf("Failed to init\n");
    } else {
        if (!loadMedia()) {
            printf("Failed to load media\n");
        } else {
            bool quit = false;
            SDL_Event e;
            gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

            while (!quit) {
                while (SDL_PollEvent(&e) != 0) {
                    if (e.type == SDL_QUIT) {
                        quit = true;
                    } else if (e.type == SDL_KEYDOWN) {
                        switch (e.key.keysym.sym) {
                            case SDLK_UP: {
                                gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
                                break;
                            }
                            case SDLK_DOWN: {
                                gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
                                break;
                            }
                            case SDLK_LEFT: {
                                gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
                                break;
                            }
                            case SDLK_RIGHT: {
                                gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
                                break;
                            }
                            default: {
                                gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
                                break;
                            }
                        }
                    }
                }
                SDL_Rect stretchRect;
                stretchRect.x = 0;
                stretchRect.y = 0;
                stretchRect.w = WIDTH;
                stretchRect.h = HEIGHT;
                SDL_BlitScaled(gCurrentSurface, NULL, gScreenSurface, &stretchRect);

                SDL_UpdateWindowSurface(gWindow);
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
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("bmps/default.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("bmps/up.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("bmps/down.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("bmps/left.bmp");
    gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("bmps/right.bmp");

    if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
        printf("Failed to load default image!\n");
        success = false;
    }
    if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL) {
        printf("Failed to load up image!\n");
        success = false;
    }
    if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL) {
        printf("Failed to load down image!\n");
        success = false;
    }
    if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL) {
        printf("Failed to load left image!\n");
        success = false;
    }
    if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL) {
        printf("Failed to load right image!\n");
        success = false;
    }

    return success;
}

void close() {
    //Deallocate surfaces
    for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i) {
        SDL_FreeSurface(gKeyPressSurfaces[i]);
        gKeyPressSurfaces[i] = NULL;
    }

    //Destroy window
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;

    //quit sdl subsystems
    SDL_Quit();
}

SDL_Surface *loadSurface(char *path) {
    SDL_Surface *optimizedSurface = NULL;
    SDL_Surface *loadedSurface = SDL_LoadBMP(path);
    if (loadedSurface == NULL) {
        printf("Surface image: %s could not be loaded! SDL_Error: %s\n", path, SDL_GetError());
    } else {
        optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
        if (optimizedSurface == NULL) {
            printf("Error optimizing surface of %s! SDL_Error: %s\n", path, SDL_GetError());
        }
        SDL_FreeSurface(loadedSurface);
        loadedSurface = NULL;
    }
    return optimizedSurface;
}
