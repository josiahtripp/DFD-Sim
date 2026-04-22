#ifndef CURVEEDITORSCREEN_H
    #define CURVEEDITORSCREEN_H

    #include "screen.h"
    #include "curve.h"
    #include <utility>
    #include <string>
    #include <cstddef>

    class CurveEditorScreen : public BaseScreen
    {
            /* The current mode of the curve editor (mode button selected) */
            enum _editor_mode
            {
                MODE_TAIL,
                MODE_HEAD,
                MODE_DELETE,
                MODE_MOVE,
                MODE_OFFSET,
                MODE_COUNT
            };

            /* The current curve selected in the editor (curve button state) */
            enum _curve_selection
            {
                CURVE_SELECTED_1,
                CURVE_SELECTED_2,
                CURVE_SELECTED_COUNT
            };

        private:
            
            /* The current mode of the curve editor */
            _editor_mode editor_mode = MODE_TAIL;
            
            /* The current curve selected */
            enum _curve_selection curve_selection;

            /* The curve objects */
            Curve curve_1;
            Curve curve_2;

            /* Mode button textures anbd associated FRects */
            SDL_Texture* mode_textures[MODE_COUNT];
            SDL_FRect mode_texture_frects[MODE_COUNT];

            /* Curve selection button texture and associated frects */
            SDL_Texture* curve_selected_textures[CURVE_SELECTED_COUNT];
            SDL_FRect curve_selected_texture_frects[CURVE_SELECTED_COUNT];

            /* Curve selection button texture and its associated FRect */
            SDL_Texture* button_selected_texture;
            SDL_FRect button_selected_texture_frect;

            /* Hovering button texture and its associated frect */
            SDL_Texture* hovering_button_texture;
            SDL_FRect hovering_button_texture_frect;

            /* Terminal button */
            SDL_Texture* terminal_input_button_texture;
            SDL_FRect terminal_input_button_texture_frect;
            SDL_Texture* hovering_terminal_input_button_texture;

            /* Editor window rect */
            const SDL_FRect editor_window_outer_frect = {0.0f, 0.0f, 800.0f, 800.0f};
            const SDL_FRect editor_window_inner_frect = {15.0f, 15.0f, 770.0f, 770.0f};
            const SDL_Rect editor_window_inner_rect = {15, 15, 770, 770};

            /* Checks if the mouse is hovering over an editor mode button.
               Returns the enum of the button or -1 is none is found */
            int isHoveringEditorModeButton();

            int isHoveringCurveSelectionButton();

            bool drawHoveringEditorModeButton();

            bool drawHoveringCurveSelectionButton();

            bool drawHoveringEditorWindow();

            bool drawHoveringTerminalInputButton();

            bool updateEditorModeClicked();

            bool updateCurveSelectionClicked();

            bool updateTerminalInputButtonClicked();

            void handleTIPoint(int x, int y);

            bool parseCoordinate(const std::string& s, size_t& pos, int& outVal);

            int parseAndExecute(const std::string& input);
            
            void handleTerminalInput();

            void renderEditorModeButtons();

            void renderCurveSelectionButtons();

            void loadTextures();

            bool updateEditorWindowClicked();

        public:
            // Constructor
            CurveEditorScreen();

            // Implemented functions
            void onHoverEvent(SDL_Event *event);
            void onUpdate();
            void onHoverUpdate();

            // Curve access function
            Curve* getCurve(int s);

            // Destructor
            ~CurveEditorScreen();
    };

#endif