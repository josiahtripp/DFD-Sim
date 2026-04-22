#include "screen.h"

bool BaseScreen::setupScreen()
{
    // Create and allocate the screen texture buffer
    this->_buffer = SDL_CreateTexture(global_renderer, 
                                SDL_PIXELFORMAT_RGBA8888,
                                SDL_TEXTUREACCESS_TARGET, 
                                this->w(),
                                this->h());

    // Check if the buffer was successfully created
    if(this->_buffer == NULL)
    {
        return false;
    }

    // Assign the floating point SDL rect struct
    this->_frect = {(float) _rect.x,(float) _rect.y, (float) _rect.w, (float) _rect.h};

    return true;
}

SDL_Rect BaseScreen::rect()
{
    return this->_rect;
}

int BaseScreen::x()
{
    return this->_rect.x;
}

int BaseScreen::y()
{
    return this->_rect.y;
}

int BaseScreen::w()
{
    return this->_rect.w;
}

int BaseScreen::h()
{
    return this->_rect.h;
}

void BaseScreen::startTargetBuffer()
{
    // Switch the target of the global renderer to the screen's buffer texture
    SDL_SetRenderTarget(global_renderer, this->_buffer);
}

void BaseScreen::endTargetBuffer()
{
    // Switch the target of the global renderer back to the global window
    SDL_SetRenderTarget(global_renderer, NULL);
}

void BaseScreen::drawBuffer()
{
    // Draw the screen texture buffer to the renderer 
    // * THE RENDERING TARGET MUST BE THE GLOBAL WINDOW *
    SDL_RenderTexture(global_renderer, this->_buffer, NULL, &this->_frect);
}

std::string BaseScreen::getAssetFilepath(std::string asset_filename)
{
    return _asset_folder + asset_filename;
}

BaseScreen::BaseScreen(int x, int y, int w, int h, std::string asset_folder)
{   
    // Set the screen rect
    this->_rect = {x, y, w, h};

    // Add the directory separating slash if not present
    if(asset_folder[asset_folder.length() - 1] != '/')
    {
        asset_folder.push_back('/');
    }

    // Set the asset folder filepath
    _asset_folder = asset_folder;

    // Setup the rest of the screen
    this->setupScreen();
}

BaseScreen::BaseScreen(int x, int y, int w, int h)
{   
    // Set the screen rect
    this->_rect = {x, y, w, h};

    // Setup the rest of the screen
    this->setupScreen();
}

/* Empty virtual function implementations */
void BaseScreen::onEvent(SDL_Event *event){}
void BaseScreen::onHoverEvent(SDL_Event *event){}
void BaseScreen::onUpdate(){}
void BaseScreen::onHoverUpdate(){}

/* Destructor */
BaseScreen::~BaseScreen()
{   
    // Free the screen's buffer texture
    SDL_DestroyTexture(this->_buffer);
}