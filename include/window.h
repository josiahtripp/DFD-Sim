#ifndef WINDOW_H
    #define WINDOW_H

    #include "metadata.h"
    #include <SDL3/SDL.h>
    #include <SDL3/SDL_video.h>

    /* Window size */
    #define WIN_W 1750
    #define WIN_H 1000

    /* Window creation flags */
    #define WIN_F 0U

    /* Window title */
    #define WIN_T APP_NAME

    /* Renderer to use */
    #define REN_NAME NULL // Default

    /* Renderer logical presentation mode */
    #define REN_LP_MODE SDL_LOGICAL_PRESENTATION_DISABLED

    /* Global SDL window and renderer pointers */
    extern SDL_Window* global_window;
    extern SDL_Renderer* global_renderer;

    /* Draw a filled circle using the global renderer */
    void drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius);

#endif