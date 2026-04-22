#ifndef DFDSCREEN_H
    #define DFDSCREEN_H

    #include "screen.h"
    #include "curve.h"

    class DfdScreen : public BaseScreen
    {

        /* The current mode of the curve editor (mode button selected) */
        enum control_mode
        {
            MODE_PLAY,
            MODE_PAUSE,
            MODE_STOP,
            MODE_COUNT
        };

        enum algorithm_stage
        {
            STAGE_TABULATION,
            STAGE_DISPLAY
        };

        private:

            // Referenced curve from the curve editor
            Curve* curve_1;
            Curve* curve_2;

            // Curve copies held for algorithm simulation
            Curve current_c1;
            Curve current_c2;

            // The tabulation matrix held for the algorithm
            std::unique_ptr<std::unique_ptr<double[]>[]> tabulation_matrix;
            
            /* The current mode of the curve editor */
            control_mode _control_mode = MODE_STOP;

            /* Control Mode button textures anbd associated FRects */
            SDL_Texture* mode_textures[MODE_COUNT];
            SDL_FRect mode_texture_frects[MODE_COUNT];

            /* Refresh button texture */
            SDL_Texture* refresh_button_texture;
            SDL_FRect refresh_button_texture_frect;

            /* Curve selection button texture and its associated FRect */
            SDL_Texture* button_selected_texture;
            SDL_FRect button_selected_texture_frect;

            /* Hovering button texture and its associated frect */
            SDL_Texture* hovering_button_texture;
            SDL_FRect hovering_button_texture_frect;

            /* Editor window rect */
            const SDL_FRect editor_window_outer_frect = {0.0f, 0.0f, 800.0f, 800.0f};
            const SDL_FRect editor_window_inner_frect = {15.0f, 15.0f, 770.0f, 770.0f};
            const SDL_Rect editor_window_inner_rect = {15, 15, 770, 770};

            /* Checks if the mouse is hovering over an editor mode button.
               Returns the enum of the button or -1 is none is found */
            int isHoveringControlModeButton();

            bool isHoveringRefreshButton();

            bool drawHoveringControlModeButton();

            bool drawHoveringRefreshButton();

            bool updateControlModeButtonsClicked();

            bool updateRefreshButtonClicked();

            void renderControlModeButtons();

            void renderRefreshButton();

            void loadTextures();

            // Update the current curves with the curves references from the editor
            void refreshCurves();

            // Completely reset the algorithm
            void resetAlgorithm(bool curves_refreshed);

            // Update the next step of the algorithm, based on the selected button (play / pause)
            void updateAlgorithm();

            // Continue the next step of the algorithm
            void stepAlgorithm();

            // Number of ticks since last update
            Uint64 _last_update;

            // Number of ticks for each step
            const Uint64 _delay_interval = 750;

            // Color to draw the overlay segment (algorithm)
            Uint8 _alg_r;
            Uint8 _alg_g;
            Uint8 _alg_b;
            
            // Points to draw (algorithm)
            SDL_Point* _alg_p1;
            SDL_Point* _alg_p2;

            // Indexes of the last points compared
            int _alg_c1_idx;
            int _alg_c2_idx;

            // Current stage of the algorithm
            enum algorithm_stage _alg_stage;
            
            // Hold the Frechet distance once calculated
            double _frechet_distance;

        public:

            // Create with reference to curves in editor screen
            DfdScreen(Curve* c1, Curve* c2);

            // Implemented functions
            void onHoverEvent(SDL_Event *event);
            void onUpdate();
            void onHoverUpdate();

            ~DfdScreen();
    };

#endif