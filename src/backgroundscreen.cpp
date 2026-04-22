#include "backgroundscreen.h"
#include <SDL3_image/SDL_image.h>
#include "window.h"

BackgroundScreen::BackgroundScreen() : BaseScreen(0, 0, WIN_W, WIN_H)
{
    /*input_curves_texture = IMG_LoadTexture(global_renderer, "../input_curves_texture.png");
    input_curves_texture_frect = {50, 900, (float) input_curves_texture->w, (float) input_curves_texture->h};
    */
}

void BackgroundScreen::onHoverUpdate()
{
    // Clear the entire background of window with white
    SDL_SetRenderDrawColor(global_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(global_renderer);

    // Draw the labels for the editor and visualization
    /*SDL_RenderTexture(global_renderer,
                      input_curves_texture, 
                      NULL, 
                      &input_curves_texture_frect);*/
}

BackgroundScreen::~BackgroundScreen()
{
   // SDL_DestroyTexture(input_curves_texture);
}