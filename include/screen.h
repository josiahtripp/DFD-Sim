#ifndef SCREENS_H
    #define SCREENS_H

    #include "window.h"
    #include <string>

    /* Abstract base class to be inherited by screen implementations */
    class BaseScreen
    {
        private:

            /* Global screen location and size */
            SDL_Rect _rect;
            SDL_FRect _frect;

            /* Asset folder filepath */
            std::string _asset_folder;

            /* Screen texture buffer */
            SDL_Texture* _buffer = nullptr;

            /* Allocates buffer texture and sets up screen */
            bool setupScreen();
        
        public:

            /* Called on event when the mouse is NOT hovering over the screen */
            virtual void onEvent(SDL_Event *event);

            /* Called on event when ther mouse IS hovering over the screen */
            virtual void onHoverEvent(SDL_Event *event);

            /* Called on frame update when the mouse is NOT hovering over the screen */
            virtual void onUpdate();

            /* Called on frame update when the mouse IS hovering over the screen */
            virtual void onHoverUpdate();

            /* Returns (a copy of) the screen's SDL rect struct */
            SDL_Rect rect();

            /* Returns the x-coordinate position of the screen */
            int x();

            /* Returns the y-coordinate position of the screen */
            int y();

            /* Returns the width size of the screen */
            int w();

            /* Returns the height size of the screen */
            int h();

            /* Sets the target of the global renderer to the screen's buffer texture */
            void startTargetBuffer();

            /* Sets the target of the global renderer to the global window */
            void endTargetBuffer();

            /* Renders the screen's buffer texture to the global renderer 
               * THE TARGET OF THE GLOBAL RENDERER MUST BE SET TO THE GLOBAL WINDOW */
            void drawBuffer();

            /* Returns the full filepath of an asset file given it's filename */
            std::string getAssetFilepath(std::string asset_filename);

            /* Constructor with asset folder */
            BaseScreen(int x, int y, int w, int h, std::string asset_folder);

            /* Constructor without asset folder */
            BaseScreen(int x, int y, int w, int h);

            /* Destructor */
            ~BaseScreen();
    };
    
#endif