#ifndef SCREENHANDLER_H
    #define SCREENHANDLER_H

    #include "screen.h"
    #include <vector>
    #include <memory>

    class ScreenHandler
    {
        private:
            std::vector<BaseScreen*> screens;

            // Helper function for retrieving mouse coordinates
            SDL_Point* getMousePoint(SDL_Point* point);
            bool isHovering(BaseScreen* screen, SDL_Point* mouse);

        public:
            ScreenHandler(std::vector<BaseScreen*>& screens);
            void event(SDL_Event *event);
            void update();
    };

    class ScreenHandlerBuilder
    {
        private:
            std::vector<BaseScreen*> screens;

        public:
            // Does not free referenced screens
            void addScreen(BaseScreen* screen);
            std::shared_ptr<ScreenHandler> build();
    };

#endif