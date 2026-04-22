/* Use callback functions instead of main() */
#define SDL_MAIN_USE_CALLBACKS 1

#include "metadata.h"
#include "screenhandler.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "window.h"
#include "curveeditorscreen.h"
#include "backgroundscreen.h"
#include "dfdscreen.h"

/* Main window and renderer handles */
SDL_Window* global_window = NULL;
SDL_Renderer* global_renderer = NULL;

/* Screens */
std::shared_ptr<DfdScreen> dfd_screen;
std::shared_ptr<CurveEditorScreen> curve_editor_screen;
std::shared_ptr<BackgroundScreen> background_screen;

/* Screen handler */
std::shared_ptr<ScreenHandler> screen_handler;

/* Create the screen handler and set it up */
static bool screenHandlerSetup()
{
    // Create the builder for the screen handler
    ScreenHandlerBuilder screen_handler_builder;

    // Create and add the background screen
    background_screen = std::make_shared<BackgroundScreen>();
    screen_handler_builder.addScreen(background_screen.get());

    // Create and add the curve editor screen
    curve_editor_screen = std::make_shared<CurveEditorScreen>();
    screen_handler_builder.addScreen(curve_editor_screen.get());

    // Create and add the dfd screen (with reference to the two curve in the editor)
    dfd_screen = std::make_shared<DfdScreen>(curve_editor_screen->getCurve(0), curve_editor_screen->getCurve(1));
    screen_handler_builder.addScreen(dfd_screen.get());

    // Build the screen handler
    screen_handler = screen_handler_builder.build();

    // Check to ensure that the screen handler was created properly
    if(screen_handler == nullptr)
    {
        return false;
    }

    return true;
}

/* Program general setup function - called once by SDL_AppInit */
static bool programSetup()
{   
    // Set up the screens and the screen handler
    if(!screenHandlerSetup())
    {
        return false;
    }

    return true;
}

/* Startup callback function - called once at startup */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    // Configure application metadata
    SDL_SetAppMetadata(APP_NAME, APP_VER, APP_DOM);

    // Initialize SDL library
    if(!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_Log("Couldn't initialize SDL library: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Create the window
    global_window = SDL_CreateWindow(WIN_T, WIN_W, WIN_H, WIN_F);

    // Window creation failed
    if (global_window == NULL) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Create the renderer
    global_renderer = SDL_CreateRenderer(global_window, REN_NAME);

    // Renderer creation failed
    if (global_renderer == NULL) {
        SDL_Log("Couldn't create renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Configure renderer for window size changes
    SDL_SetRenderLogicalPresentation(global_renderer, WIN_W, WIN_H, REN_LP_MODE);

    // Setup the rest of the program
    if(!programSetup())
    {
        return SDL_APP_FAILURE;
    }

    // Continue on with the program
    return SDL_APP_CONTINUE;
}

/* Event callback function - called on event */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // Exit if the window was closed
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    // Let the screen handler deal with all other events
    screen_handler->event(event);

    // Continue on with the program
    return SDL_APP_CONTINUE;
}

/* Frame update callback function - called every frame */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    // Invoke update on the screen handler
    screen_handler->update();

    // Continue on with the program
    return SDL_APP_CONTINUE;
}

/* Exit callback function - called at end of program */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{

}

void drawCircle(SDL_Renderer* renderer, int32_t centreX, int32_t centreY, int32_t radius) {
   const int32_t diameter = (radius * 2);
   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y) {
      SDL_RenderLine(renderer, (float)(centreX - x), (float)(centreY - y), (float)(centreX + x), (float)(centreY - y));
      SDL_RenderLine(renderer, (float)(centreX - x), (float)(centreY + y), (float)(centreX + x), (float)(centreY + y));
      SDL_RenderLine(renderer, (float)(centreX - y), (float)(centreY - x), (float)(centreX + y), (float)(centreY - x));
      SDL_RenderLine(renderer, (float)(centreX - y), (float)(centreY + x), (float)(centreX + y), (float)(centreY + x));

      if (error <= 0) {
         y++;
         error += ty;
         ty += 2;
      }
      if (error > 0) {
         x--;
         tx += 2;
         error += (tx - diameter);
      }
   }
}