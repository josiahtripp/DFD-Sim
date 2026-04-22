#ifndef BACKGROUND_SCREEN
    #define BACKGROUND_SCREEN

    #include "screen.h"

    class BackgroundScreen : public BaseScreen
    {
        public:
            // Constructor
            BackgroundScreen();

            // Implemented functions
            void onHoverUpdate();

            // Destructor
            ~BackgroundScreen();
    };

#endif