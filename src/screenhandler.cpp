#include "screenhandler.h"
#include "window.h"

SDL_Point* ScreenHandler::getMousePoint(SDL_Point* point)
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    point->x = (int) x;
    point->y = (int) y;

    return point;
}

void ScreenHandlerBuilder::addScreen(BaseScreen* screen)
{
    this->screens.push_back(screen);
}

std::shared_ptr<ScreenHandler> ScreenHandlerBuilder::build()
{
    return std::make_shared<ScreenHandler>(screens);
}

ScreenHandler::ScreenHandler(std::vector<BaseScreen*>& screens)
{
    this->screens = screens;
}

bool ScreenHandler::isHovering(BaseScreen* screen, SDL_Point* mouse)
{
    SDL_Rect screen_rect = screen->rect();
    return(SDL_PointInRect(mouse, &screen_rect));
}

void ScreenHandler::event(SDL_Event *event)
{
    // Get mouse position
    SDL_Point mouse;
    this->getMousePoint(&mouse);

    for(BaseScreen* scr : this->screens)
    {
        if(this->isHovering(scr, &mouse))
        {
            scr->onHoverEvent(event);
        }
        else
        {
            scr->onEvent(event);
        }
    }
}

void ScreenHandler::update()
{
    // Get mouse position
    SDL_Point mouse;
    this->getMousePoint(&mouse);

    // Update each screen appropriately
    for(BaseScreen* scr : this->screens)
    {
        scr->startTargetBuffer();

        if(this->isHovering(scr, &mouse))
        {
            scr->onHoverUpdate();
        }
        else
        {
            scr->onUpdate();
        }

        scr->endTargetBuffer();
    }

    // Finally, clear the renderer with black
    SDL_SetRenderDrawColor(global_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(global_renderer);

    // Render each screen texture
    for(BaseScreen* scr : this->screens)
    {
        scr->drawBuffer();
    }

    // Draw the textures to the window
    SDL_RenderPresent(global_renderer);
}